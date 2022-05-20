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

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual void NextInventoryItem();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual void PrevInventoryItem();

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class AWeapon* CurrentInventoryItem;

	class ATestFireCharacter* m_Owner;

	uint16 m_MaxStoredAmmo;
	uint16 m_StoredPistolAmmo;
	uint16 m_StoredRifleAmmo;

	void EquipNewInventoryItem(AWeapon* NewItem);
	void SelectInventoryItem(EInventoryDirection Direction);

};
