// Test Fire is a simple 3D shooter created by Wilson Worlds, intended to build familiarity with the unreal engine and game design for 'Shooters'. June 22nd, 2021.

#pragma once

#include "CoreMinimal.h"
#include "TestFire/Items/Item.h"
#include "Weapon.generated.h"


UCLASS()
class TESTFIRE_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
		float AttackRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int MaxAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int CurrentAmmo;

	UPROPERTY(EditDefaultsOnly)
		FVector TraceDirection;

	UPROPERTY(EditDefaultsOnly)
		class UClass* ProjectileType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
		class USoundBase* ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
		class USoundBase* NoAmmoSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
		class USoundBase* FireProjectileSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
		UParticleSystem* FireProjectileFX;

	void Equip(class ATestFireCharacter* Character);
	void UnEquip();
	void Drop();

	void AttachToBack(class ATestFireCharacter* Character);

	void OnTriggerPull();
	void OnTriggerRelease();

	void PlayReloadEffects();
	void PlayProjectileEffects();

	FVector GetMuzzleLocation() const;
	int GetCurrentAmmo();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess), Category = "Weapon")
		class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, Category = "Trace")
		float TraceDistance;

	AWeapon();
	virtual void PostInitializeComponents() override;

	UFUNCTION()
		void SetFireDirection();

	UFUNCTION()
		void Fire();

	void SpawnProjectile();

private:
	class UArrowComponent* Muzzle;
	FTimerHandle AttackTimer;

	void ClearAttackTimer();
};
