// Fill out your copyright notice in the Description page of Project Settings.


#include "TestFire/Actors/BasicTurret.h"
#include "TestFire/Characters/TestFireCharacter.h"
#include "TestFire/Items/Weapon/Projectile.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GamePlayStatics.h"

// Sets default values
ABasicTurret::ABasicTurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Addition character properties
	Health = 100.0f;
	PointValue = 100;
	ReloadTime = 2.0f;
	bIsReloading = false;
	bIsActive = false;

	// Mesh
	BaseTurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseTurretMesh");
	SetRootComponent(BaseTurretMesh);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
	TurretMesh->SetupAttachment(RootComponent);

	// Target 1 Component defaults setup
	ActivationBox = CreateDefaultSubobject<UBoxComponent>("Activate Box");
	ActivationBox->SetSimulatePhysics(false);
	ActivationBox->SetCollisionProfileName("OverlapAllDynamic");
	ActivationBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ActivationBox->SetupAttachment(RootComponent);

	// Trigger function to take damage when struck by a damage event causing actor
	OnTakeAnyDamage.AddDynamic(this, &ABasicTurret::TakeAnyDamage);

	ActivationBox->OnComponentBeginOverlap.AddDynamic(this, &ABasicTurret::BeginOverlap);
	ActivationBox->OnComponentEndOverlap.AddDynamic(this, &ABasicTurret::EndOverlap);
}

void ABasicTurret::PostInitializeComponents() {
	Super::PostInitializeComponents();

	// Get the Muzzle's arrow component
	auto& Components = GetComponents();

	for (auto Component : Components) {
		if (Component->GetFName() == "Muzzle") {
			Muzzle = Cast<UArrowComponent>(Component);
			break;
		}
	}
}

// Called when the game starts or when spawned
void ABasicTurret::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasicTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive == false || bIsReloading == true)
		return;

	FireWeapon();
}

void ABasicTurret::BeginOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this)
	{
		ATestFireCharacter* TFActor = Cast<ATestFireCharacter>(OtherActor);

		if (TFActor)
		{
			bIsActive = true;
		}
	}
}

void ABasicTurret::EndOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherActor != this)
	{
		ATestFireCharacter* TFActor = Cast<ATestFireCharacter>(OtherActor);

		if (TFActor)
		{
			bIsActive = false;
		}
	}
}

void ABasicTurret::TakeAnyDamage(AActor* damagedActor, float Damage, const UDamageType* damageType, AController* InstigatedBy, AActor* damageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("taken %.2f damage"), Damage));

	Health -= Damage;

	if (Health <= 0)
	{
		IncreasePlayerScore();
		Destroy();
	}
}

void ABasicTurret::IncreasePlayerScore()
{
	AController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn* pPlayerPawn = pController->GetPawn();
	ATestFireCharacter* pPlayer = Cast<ATestFireCharacter>(pPlayerPawn);

	if (pPlayer)
	{
		pPlayer->Score += PointValue;
	}
}

void ABasicTurret::FireWeapon()
{
	SpawnProjectile();
	bIsReloading = true;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ABasicTurret::ClearReloadTimer, ReloadTime, false);

	PlayProjectileEffects();
}

void ABasicTurret::SpawnProjectile()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	FTransform Transform = Muzzle->GetComponentToWorld();
	FVector moveDir = (Muzzle->GetForwardVector() + Transform.GetLocation()) + Muzzle->GetForwardVector() * 10000.f;
	AProjectile* projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileType, Transform.GetLocation(), Transform.GetRotation().Rotator(), SpawnParams);
	projectile->SetMovementDirection(moveDir);
}

void ABasicTurret::PlayProjectileEffects()
{
	if (ProjectileSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileSound, GetActorLocation());
	}

	if (ProjectileFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ProjectileFX, GetMuzzleLocation());
	}
}

void ABasicTurret::ClearReloadTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	bIsReloading = false;
}

FVector ABasicTurret::GetMuzzleLocation() const
{
	return Muzzle->GetComponentToWorld().GetLocation();
}
