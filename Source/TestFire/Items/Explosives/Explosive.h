#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Explosive.generated.h"

UCLASS()
class TESTFIRE_API AExplosive : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
		class UStaticMeshComponent* ExplosiveMesh;

	UPROPERTY(EditAnywhere, Category = "Explosive")
		class UProjectileMovementComponent* ExplosiveMovementComp;

	UPROPERTY(EditAnywhere, Category = "Explosive")
		class URadialForceComponent* ExplosiveBlastComp;

	UPROPERTY(EditAnywhere, Category = "Explosive")
		class UParticleSystem* ExplosiveFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
		class USoundBase* PinPullSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
		class USoundBase* ExplosionSound;

	FTimerHandle FuseTimerHandle;
	float FuseTime;

	FTimerHandle LifespanTimerHandle;
	float LifespanTime;

	AExplosive();

	UFUNCTION()
		void OnDestruction();

	UFUNCTION()
		void OnExplosion();

	UFUNCTION()
		void PlayExplosionFX();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
		TSubclassOf<UDamageType> DamageClass;

	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
		float BlastDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
		float BlastRadius;

	bool bHasImpacted;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector Impulse, const FHitResult& Hit);

	TArray<AActor*> RayCastSphere_GetActors();

	virtual void BeginPlay() override;

};
