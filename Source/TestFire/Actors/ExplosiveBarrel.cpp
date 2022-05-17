// Fill out your copyright notice in the Description page of Project Settings.


#include "TestFire/Actors/ExplosiveBarrel.h"
#include "Kismet/GamePlayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = 15.0f;
	BlastDamage = 100.0f;
	BlastRadius = 600.0f;
	LifeTime = 1.0f;
	bHasExploded = false;

	// Mesh
	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>("BarrelMesh");
	BarrelMesh->SetCollisionProfileName("BlockAll");
	BarrelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BarrelMesh->SetSimulatePhysics(true);
	SetRootComponent(BarrelMesh);

	// Radial Force
	RadialBlastComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("ExplosiveBlastRadius"));
	RadialBlastComp->SetupAttachment(RootComponent);
	RadialBlastComp->AddCollisionChannelToAffect(ECollisionChannel::ECC_Pawn);
	RadialBlastComp->AddCollisionChannelToAffect(ECollisionChannel::ECC_WorldDynamic);
	RadialBlastComp->AddCollisionChannelToAffect(ECollisionChannel::ECC_WorldStatic);
	RadialBlastComp->AddCollisionChannelToAffect(ECollisionChannel::ECC_PhysicsBody);
	RadialBlastComp->bImpulseVelChange = true;
	RadialBlastComp->ImpulseStrength = 1000.0f;
	RadialBlastComp->Radius = BlastRadius;

	// Trigger function to take damage when struck by a damage event causing actor
	OnTakeAnyDamage.AddDynamic(this, &AExplosiveBarrel::TakeAnyDamage);
}

// Called when the game starts or when spawned
void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosiveBarrel::TakeAnyDamage(AActor* damagedActor, float Damage, const UDamageType* damageType, AController* InstigatedBy, AActor* damageCauser)
{
	Health -= Damage;

	if (Health <= 0 && bHasExploded == false)
	{
		bHasExploded = true;
		OnExplosion();
	}
}

void AExplosiveBarrel::OnExplosion()
{
	BarrelMesh->SetStaticMesh(nullptr);
	PlayExplosionFX();
	RadialBlastComp->FireImpulse();

	TArray<AActor*> HitActors = RayCastSphere_GetActors();
	for (AActor* actor : HitActors)
	{
		UGameplayStatics::ApplyDamage(actor, BlastDamage, GetWorld()->GetFirstPlayerController(), this->GetOwner(), DamageClass);
	}

	GetWorldTimerManager().SetTimer(LifeTimerHandle, this, &AExplosiveBarrel::DestroyBarrel, LifeTime);
}

void AExplosiveBarrel::PlayExplosionFX()
{
	if (ExplosionFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, this->GetActorLocation());
	}

	if (ExplodingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplodingSound, this->GetActorLocation());
	}
}

void AExplosiveBarrel::DestroyBarrel()
{
	Destroy();
}

TArray<AActor*> AExplosiveBarrel::RayCastSphere_GetActors()
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