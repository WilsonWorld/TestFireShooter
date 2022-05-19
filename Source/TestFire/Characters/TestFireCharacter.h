// Copyright Epic Games, Inc. All Rights Reserved.
// Test Fire is a simple 3D shooter created by Wilson Worlds, intended to build familiarity with the unreal engine and game design for 'Shooters'. June 22nd, 2021.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TestFireCharacter.generated.h"

UENUM(BlueprintType)
namespace ECharacterCameraMode
{
	enum Type
	{
		ThirdPersonDefault				UMETA(DisplayName = "Third Person Default"),
		ThirdPersonOverShoulder		UMETA(DisplayName = "Third Person Shoulder"),
		COUNT								UMETA(Hidden)
	};
}

UCLASS(config=Game)
class ATestFireCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
		float MaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly)
		float MaxJogSpeed;

	UPROPERTY(EditDefaultsOnly)
		float MaxInteractDistance;

	UPROPERTY(EditDefaultsOnly)
		float CurrentHealth;

	UPROPERTY(EditDefaultsOnly)
		float MaxHealth;

	UPROPERTY(EditDefaultsOnly)
		uint32 Score;

	UPROPERTY(EditDefaultsOnly)
		uint8 CurrentExplosivesCount;

	UPROPERTY(EditDefaultsOnly)
		uint8 MaxExplosivesCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AExplosive> ExplosiveToUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		TEnumAsByte<ECharacterCameraMode::Type> CameraModeEnum;

	UFUNCTION(BlueprintPure)
		class AWeapon* GetCurrentWeapon();

	void SetGrenades(uint8 NumGrenades);
	void PullTrigger();
	void ReleaseTrigger();
	void Interact();

	/* Returns SkeletalMesh subobject */
	FORCEINLINE class UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	/* Returns SkeletalMesh subobject */
	FORCEINLINE class USlowTimeComponent* GetSlowTimeComponent() const { return SlowTimeComponent; }
	/* Returns SkeletalMesh subobject */
	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMesh() const { return SkeletalMesh; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UInventoryComponent* InventoryComponent;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USlowTimeComponent* SlowTimeComponent;

	UPROPERTY(Category = Character, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* PickupSphereComponent;

	ATestFireCharacter();
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void TakeAnyDamage(AActor* damagedActor, float Damage, const class UDamageType* damageType, class AController* InstigatedBy, AActor* damageCauser);

	UFUNCTION()
		void StartReload();

	UFUNCTION()
		void ReloadWeapon();

	UFUNCTION()
		void ClearReloadTimer();

	UFUNCTION()
		void NextWeapon();

	UFUNCTION()
		void PrevWeapon();

	UFUNCTION()
		void ThrowGrenade();

	UFUNCTION()
		void StartSlowTime();

	UFUNCTION()
		void StopSlowTime();

	UFUNCTION()
		void EndAndExitGame();

	void CycleCamera();
	void SetCameraMode(ECharacterCameraMode::Type newCameraMode);
	void UpdateForCameraMode();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/* Called via input to turn at a given rate. 
	Param Rate is a normalized rate, i.e. 1.0 means 100% of desired turn rate */
	void TurnAtRate(float Rate);

	/* Called via input to turn look up/down at a given rate. 
	 Param Rate is a normalized rate, i.e. 1.0 means 100% of desired turn rate */
	void LookUpAtRate(float Rate);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Camera boom positioning the follow camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** Over the shoulder camera boom, positioning the shoulder camera behind the character's shoulder */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* ShoulderCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* ShoulderCamera;

	UPROPERTY()
		class AWeapon* CurrentWeapon;

	class USkeletalMeshComponent* SkeletalMesh;
	class UTestFireCharacterAnimation* AnimationInstance;
	FTimerHandle ReloadTimer;
	float ReloadTime;
	bool bIsFiring;
	bool bIsReloading;
};

