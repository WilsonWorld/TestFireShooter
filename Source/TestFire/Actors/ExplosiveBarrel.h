#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

UCLASS()
class TESTFIRE_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	/***   Variables   ***/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetMesh")
		class UStaticMeshComponent* BarrelMesh;

	UPROPERTY(EditAnywhere, Category = "Explosive")
		class URadialForceComponent* RadialBlastComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
		UParticleSystem* ExplosionFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetAudio")
		class USoundBase* ExplodingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
		UParticleSystem* FireFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetAudio")
		class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetHealth")
		float Health;

	FTimerHandle LifeTimerHandle;
	float LifeTime;
	bool bHasExploded;

	/***   Functions   ***/
	AExplosiveBarrel();

	UFUNCTION()
		void OnExplosion();

	UFUNCTION()
		void PlayExplosionFX();

	UFUNCTION()
		void PlayFireFX();

	UFUNCTION()
		void DestroyBarrel();

protected:
	/***   Variables   ***/
	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
		TSubclassOf<UDamageType> DamageClass;

	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
		float BlastDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
		float BlastRadius;

	/***   Functions   ***/
	UFUNCTION()
		void TakeAnyDamage(AActor* damagedActor, float Damage, const class UDamageType* damageType, class AController* InstigatedBy, AActor* damageCauser);

	TArray<AActor*> RayCastSphere_GetActors();
};
