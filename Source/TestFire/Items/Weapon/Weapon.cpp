// Fill out your copyright notice in the Description page of Project Settings.
// Test Fire is a simple 3D shooter created by Wilson Worlds, intended to build familiarity with the unreal engine and game design for 'Shooters'. June 22nd, 2021.

#include "TestFire/Items/Weapon/Weapon.h"
#include "TestFire/Items/Weapon/Projectile.h"
#include "TestFire/Characters/TestFireCharacter.h"

#include "Components/ArrowComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


AWeapon::AWeapon() :
	Muzzle(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	// Set general properties
	AttackRate = 10.0f;
	MaxAmmo = 30.0f;
	CurrentAmmo = -1;
	TraceDistance = 10000.0f;
	TraceDirection = GetActorForwardVector();

	// Set the Mesh
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	Tags.Add("Weapon");
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Get the Muzzle's arrow component
	auto& Components = GetComponents();

	for (auto Component : Components)
	{
		if (Component->GetFName() == "Muzzle")
		{
			Muzzle = Cast<UArrowComponent>(Component);
			break;
		}
	}

	// Initialize the weapon's properties
	MaxAmmo = FMath::Max(1, MaxAmmo);
	CurrentAmmo = FMath::Min(CurrentAmmo, MaxAmmo);

	if (CurrentAmmo < 0)
	{
		CurrentAmmo = MaxAmmo;
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::Equip(ATestFireCharacter* Character)
{
	this->Enable();
	this->SetOwner(Character);
	SetActorEnableCollision(false);
	SkeletalMesh->SetSimulatePhysics(false);

	this->AttachToComponent(Character->GetSkeletalMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "GripPoint");
}

void AWeapon::UnEquip()
{
	OnTriggerRelease();

	this->Disable();
	this->SetOwner(NULL);

	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void AWeapon::Drop()
{
	OnTriggerRelease();

	this->Enable();
	this->SetOwner(NULL);
	SetActorEnableCollision(true);
	SkeletalMesh->SetSimulatePhysics(true);

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void AWeapon::AttachToBack(ATestFireCharacter* Character)
{
	SetOwner(Character);
	SetActorEnableCollision(false);
	SkeletalMesh->SetSimulatePhysics(false);

	AttachToComponent(Character->GetSkeletalMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "HolsterPoint");
}

void AWeapon::OnTriggerPull()
{
	auto& TimerManager = GetWorld()->GetTimerManager();

	if (CurrentAmmo > 0)
	{
		float RemainingTime = FMath::Max(TimerManager.GetTimerRemaining(AttackTimer), 0.0f);
		TimerManager.SetTimer(AttackTimer, this, &AWeapon::Fire, 1.0f / AttackRate, true, RemainingTime);
	}
	else if (CurrentAmmo == 0)
	{
		if (NoAmmoSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, NoAmmoSound, GetActorLocation());
		}
	}
}

void AWeapon::OnTriggerRelease()
{
	auto& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.TimerExists(AttackTimer))
	{
		float RemainingTime = TimerManager.GetTimerRemaining(AttackTimer);
		TimerManager.SetTimer(AttackTimer, this, &AWeapon::ClearAttackTimer, RemainingTime, false);
	}
}

void AWeapon::PlayReloadEffects()
{
	if (ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSound, GetActorLocation());
	}
}

void AWeapon::PlayProjectileEffects()
{
	if (FireProjectileSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireProjectileSound, GetActorLocation());
	}

	if (FireProjectileFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, FireProjectileFX, GetMuzzleLocation());
	}
}

void AWeapon::SetFireDirection()
{
	FVector CamLoc;
	FRotator CamRot;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CamLoc, CamRot);

	// First Raycast
	FVector Start = CamLoc;
	FVector End = ((CamRot.Vector() * TraceDistance) + Start);
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATestFireCharacter::StaticClass(), ActorsToIgnore);

	bool bHit = UKismetSystemLibrary::LineTraceSingle(this, Start, End, UEngineTypes::ConvertToTraceType(ECC_Camera),
		false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true, FLinearColor::Yellow, FLinearColor::White, 0.1f);

	if (bHit)
	{
		TraceDirection = HitResult.ImpactPoint;
	}
	else
	{
		TraceDirection = End;
	}

	// Second Raycast
	FVector Start2 = GetActorLocation();
	FVector End2 = ((TraceDirection - Start2) * TraceDistance);
	FHitResult HitResult2;
	TArray<AActor*> ActorsToIgnore2;
	ActorsToIgnore2 = ActorsToIgnore;

	bool bHit2 = UKismetSystemLibrary::LineTraceSingle(this, Start2, End2, UEngineTypes::ConvertToTraceType(ECC_Camera),
		false, ActorsToIgnore2, EDrawDebugTrace::ForDuration, HitResult2, true, FLinearColor::Yellow, FLinearColor::White, 0.1f);

	if (bHit2)
	{
		TraceDirection = HitResult2.ImpactPoint;
	}
	else
	{
		TraceDirection = End2;
	}
}

void AWeapon::Fire()
{
	if (CurrentAmmo > 0)
	{
		SetFireDirection();
		SpawnProjectile();
		PlayProjectileEffects();
		CurrentAmmo--;
	}
}

void AWeapon::SpawnProjectile()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	FTransform Transform = Muzzle->GetComponentToWorld();

	AProjectile* projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileType, Transform.GetLocation(), Transform.GetRotation().Rotator(), SpawnParams);

	projectile->SetMovementDirection(TraceDirection);
}

void AWeapon::ClearAttackTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
}

FVector AWeapon::GetMuzzleLocation() const
{
	return Muzzle->GetComponentToWorld().GetLocation();
}

int AWeapon::GetCurrentAmmo()
{
	return CurrentAmmo;
}
