// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"


AExplosive::AExplosive()
{
 	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	// Additional Properties
	BlastDamage = 100.0f;
	BlastRadius = 600.0f;
	FuseTime = 3.0f;
	LifespanTime = 1.0f;
	bHasImpacted = false;

	// Mesh
	ExplosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMesh"));
	ExplosiveMesh->SetCollisionProfileName("BlockAll");
	ExplosiveMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ExplosiveMesh->SetSimulatePhysics(true);
	SetRootComponent(ExplosiveMesh);
	ExplosiveMesh->OnComponentHit.AddDynamic(this, &AExplosive::OnHit);

	// Movement
	ExplosiveMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ExplosiveMovement"));
	ExplosiveMovementComp->SetUpdatedComponent(RootComponent);
	ExplosiveMovementComp->InitialSpeed = 2000.0f;
	ExplosiveMovementComp->MaxSpeed = 2000.0f;
	ExplosiveMovementComp->bRotationFollowsVelocity = true;
	ExplosiveMovementComp->ProjectileGravityScale = 1.0f;

	// Radial Force
	ExplosiveBlastComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("ExplosiveBlastRadius"));
	ExplosiveBlastComp->SetupAttachment(RootComponent);
	ExplosiveBlastComp->AddCollisionChannelToAffect(ECollisionChannel::ECC_Pawn);
	ExplosiveBlastComp->AddCollisionChannelToAffect(ECollisionChannel::ECC_WorldDynamic);
	ExplosiveBlastComp->AddCollisionChannelToAffect(ECollisionChannel::ECC_WorldStatic);
	ExplosiveBlastComp->AddCollisionChannelToAffect(ECollisionChannel::ECC_PhysicsBody);
	ExplosiveBlastComp->bImpulseVelChange = true;
	ExplosiveBlastComp->ImpulseStrength = 1000.0f;
	ExplosiveBlastComp->Radius = BlastRadius;
}

void AExplosive::OnDestruction()
{
	Destroy();
}

void AExplosive::OnExplosion()
{
	PlayExplosionFX();
	ExplosiveBlastComp->FireImpulse();

	TArray<AActor*> HitActors = RayCastSphere_GetActors();
	for (AActor* actor : HitActors)
	{
		UGameplayStatics::ApplyDamage(actor, BlastDamage, GetWorld()->GetFirstPlayerController(), this->GetOwner(), DamageClass);
	}

	GetWorldTimerManager().SetTimer(LifespanTimerHandle, this, &AExplosive::OnDestruction, LifespanTime);
}

void AExplosive::PlayExplosionFX()
{
	if (ExplosiveFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosiveFX, this->GetActorLocation());
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, this->GetActorLocation());
	}
}

void AExplosive::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector Impulse, const FHitResult& Hit)
{
	if (bHasImpacted == false)
	{
		bHasImpacted = true;
		GetWorldTimerManager().SetTimer(FuseTimerHandle, this, &AExplosive::OnExplosion, FuseTime);
	}
}

TArray<AActor*> AExplosive::RayCastSphere_GetActors()
{
	UWorld* World = GetWorld();
	const FVector Position = this->GetActorLocation();
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	UClass* ActorFilter = nullptr;
	const TArray<AActor*> IgnoredActors;
	TArray<AActor*> HitActors;

	UKismetSystemLibrary::SphereOverlapActors(World, Position, BlastRadius, ObjectTypes, ActorFilter, IgnoredActors, HitActors);

	return HitActors;
}

void AExplosive::BeginPlay()
{
	Super::BeginPlay();

	if (PinPullSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PinPullSound, GetActorLocation());
	}
}
