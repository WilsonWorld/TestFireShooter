#include "TestFire/Actors/ExplosiveBarrel.h"
#include "Kismet/GamePlayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

AExplosiveBarrel::AExplosiveBarrel()
{
	// Barrel Properties
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

void AExplosiveBarrel::TakeAnyDamage(AActor* damagedActor, float Damage, const UDamageType* damageType, AController* InstigatedBy, AActor* damageCauser)
{
	Health -= Damage;

	if (Health <= 0 && bHasExploded == false) {
		bHasExploded = true;
		PlayFireFX();
		FTimerHandle ExplosionTimer;
		float ExplosionTime = 3.0f;
		GetWorldTimerManager().SetTimer(ExplosionTimer, this, &AExplosiveBarrel::OnExplosion, ExplosionTime);
	}
}

// Remove the mesh, fires a blast impulse, and performs a spherecast to gather all actors in proximity of the explosion to damage them
void AExplosiveBarrel::OnExplosion()
{
	BarrelMesh->SetStaticMesh(nullptr);
	RadialBlastComp->FireImpulse();
	PlayExplosionFX();
	TArray<AActor*> HitActors = RayCastSphere_GetActors();

	for (AActor* actor : HitActors) {
		UGameplayStatics::ApplyDamage(actor, BlastDamage, GetWorld()->GetFirstPlayerController(), this->GetOwner(), DamageClass);
	}

	GetWorldTimerManager().SetTimer(LifeTimerHandle, this, &AExplosiveBarrel::DestroyBarrel, LifeTime);
}

// Play the explosion particle effect and sound effect, if they exist / are set in BP
void AExplosiveBarrel::PlayExplosionFX()
{
	if (ExplosionFX)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, this->GetActorLocation());

	if (ExplodingSound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplodingSound, this->GetActorLocation());
}

// Play the fire particle effect and sound effect, if they exist / are set in BP
void AExplosiveBarrel::PlayFireFX()
{
	if (FireFX)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireFX, this->GetActorLocation() + this->GetActorUpVector() * 150.0f);

	if (FireSound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, this->GetActorLocation());
}

// Performs a sphere cast and returns all the actors within the radius if not set to be ignored
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

void AExplosiveBarrel::DestroyBarrel()
{
	Destroy();
}