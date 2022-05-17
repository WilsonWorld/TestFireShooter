// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

UCLASS()
class TESTFIRE_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetMesh")
		class UStaticMeshComponent* BarrelMesh;

	UPROPERTY(EditAnywhere, Category = "Explosive")
		class URadialForceComponent* RadialBlastComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
		UParticleSystem* ExplosionFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetAudio")
		class USoundBase* ExplodingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetHealth")
		float Health;

	FTimerHandle LifeTimerHandle;
	float LifeTime;
	bool bHasExploded;

	// Sets default values for this actor's properties
	AExplosiveBarrel();

	UFUNCTION()
		void OnExplosion();

	UFUNCTION()
		void PlayExplosionFX();

	UFUNCTION()
		void DestroyBarrel();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
		TSubclassOf<UDamageType> DamageClass;

	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
		float BlastDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
		float BlastRadius;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void TakeAnyDamage(AActor* damagedActor, float Damage, const class UDamageType* damageType, class AController* InstigatedBy, AActor* damageCauser);

	TArray<AActor*> RayCastSphere_GetActors();
};
