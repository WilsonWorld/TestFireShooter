// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasicTurret.generated.h"

UCLASS()
class TESTFIRE_API ABasicTurret : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurretMesh")
		class UStaticMeshComponent* BaseTurretMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurretMesh")
		class UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pathing", meta = (AllowPrivateAccess))
		class UBoxComponent* ActivationBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurretHealth")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurretValue")
		uint8 PointValue;

	UPROPERTY(EditDefaultsOnly)
		class UClass* ProjectileType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
		class USoundBase* ProjectileSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
		UParticleSystem* ProjectileFX;

	void IncreasePlayerScore();
	virtual void FireWeapon();
	void SpawnProjectile();
	void PlayProjectileEffects();

	FVector GetMuzzleLocation() const;

protected:
	class UArrowComponent* Muzzle;

	FTimerHandle ReloadTimerHandle;
	float ReloadTime;
	bool bIsReloading;
	bool bIsActive;

	ABasicTurret();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void ClearReloadTimer();

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void EndOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void TakeAnyDamage(AActor* damagedActor, float Damage, const class UDamageType* damageType, class AController* InstigatedBy, AActor* damageCauser);

};
