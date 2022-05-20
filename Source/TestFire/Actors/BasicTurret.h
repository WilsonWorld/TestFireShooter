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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess))
		class UBoxComponent* ActivationBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
		float YawValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
		float ReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
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
	bool bIsReloading;
	bool bIsActive;
	bool bRightRotation;

	ABasicTurret();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void RotateTurretRight();
	void RotateTurretLeft();
	void ClearReloadTimer();

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void EndOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void TakeAnyDamage(AActor* damagedActor, float Damage, const class UDamageType* damageType, class AController* InstigatedBy, AActor* damageCauser);

};
