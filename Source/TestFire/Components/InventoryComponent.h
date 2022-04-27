// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


UENUM(BlueprintType)
enum class EInventoryDirection : uint8
{
	NEXT = 0	UMETA(DisplayName = "NEXT"),
	PREV = 1	UMETA(DisplayName = "Prev")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTFIRE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(VisibleAnywhere)
		TArray<class AWeapon*> Inventory;

	// Cycle through inventory to equip the next item
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual void NextInventoryItem();

	// Cycle through inventory to equip the previous item
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual void PrevInventoryItem();

	// Get the currently equipped item
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		class AWeapon* GetCurrentInventoryItem() const;

	UInventoryComponent();

	void AddToInventory(class AWeapon* pItem);

	void ResetCurrentInventoryItem();

	int GetInventoryCount();

	void AddAmmoToPistolStorage(uint16 Ammo);
	void RemoveAmmoFromPistolStorage(uint16 Ammo);

	void AddAmmoToRifleStorage(uint16 Ammo);
	void RemoveAmmoFromRifleStorage(uint16 Ammo);

	uint16 GetMaxAmmoStorage();
	uint16 GetStoredPistolAmmo();
	uint16 GetStoredRifleAmmo();

protected:
	// Currently equipped item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class AWeapon* CurrentInventoryItem;

	class ATestFireCharacter* m_Owner;

	uint16 m_MaxStoredAmmo;
	uint16 m_StoredPistolAmmo;
	uint16 m_StoredRifleAmmo;

	// Called when the game starts
	virtual void BeginPlay() override;

	void EquipNewInventoryItem(AWeapon* NewItem);

	void SelectInventoryItem(EInventoryDirection Direction);

};
