#include "InventoryComponent.h"
#include "TestFire/Items/Weapon/Weapon.h"
#include "TestFire/Characters/TestFireCharacter.h"

void UInventoryComponent::NextInventoryItem()
{
	SelectInventoryItem(EInventoryDirection::NEXT);
}

void UInventoryComponent::PrevInventoryItem()
{
	SelectInventoryItem(EInventoryDirection::PREV);
}

AWeapon* UInventoryComponent::GetCurrentInventoryItem() const
{
	return CurrentInventoryItem;
}

UInventoryComponent::UInventoryComponent()
{
	m_MaxStoredAmmo = 300;
	m_StoredPistolAmmo = 0;
	m_StoredRifleAmmo = 0;
}

// Adds a weapon item to the player's storage / inventory and disable the game object in the world
void UInventoryComponent::AddToInventory(AWeapon* pItem)
{
	Inventory.AddUnique(pItem);
	pItem->Disable();
}

void UInventoryComponent::ResetCurrentInventoryItem()
{
	CurrentInventoryItem = nullptr;
}

int UInventoryComponent::GetInventoryCount()
{
	return Inventory.Num();
}

// Add pistol ammo pick ups to storage counter until the max is hit
void UInventoryComponent::AddAmmoToPistolStorage(uint16 Ammo)
{
	m_StoredPistolAmmo += Ammo;

	if (GetStoredPistolAmmo() > GetMaxAmmoStorage())
		m_StoredPistolAmmo = m_MaxStoredAmmo;
}

// Remove pistol ammo pick ups from storage counter until there is zero ammo remaining
void UInventoryComponent::RemoveAmmoFromPistolStorage(uint16 Ammo)
{
	m_StoredPistolAmmo -= Ammo;

	if (GetStoredPistolAmmo() <= 0)
		m_StoredPistolAmmo = 0;
}

// Add rifle ammo pick ups to storage counter until the max is hit
void UInventoryComponent::AddAmmoToRifleStorage(uint16 Ammo)
{
	m_StoredRifleAmmo += Ammo;

	if (GetStoredRifleAmmo() > GetMaxAmmoStorage())
		m_StoredRifleAmmo = m_MaxStoredAmmo;
}

// Remove rifle ammo pick ups from storage counter until there is zero ammo remaining
void UInventoryComponent::RemoveAmmoFromRifleStorage(uint16 Ammo)
{
	m_StoredRifleAmmo -= Ammo;

	if (GetStoredRifleAmmo() <= 0)
		m_StoredPistolAmmo = 0;
}

uint16 UInventoryComponent::GetMaxAmmoStorage()
{
	return m_MaxStoredAmmo;
}

uint16 UInventoryComponent::GetStoredPistolAmmo()
{
	return m_StoredPistolAmmo;
}

uint16 UInventoryComponent::GetStoredRifleAmmo()
{
	return m_StoredRifleAmmo;
}

// Removes the old weapon from the player and disables it before enabling the new weapon and attaching it to the player, and updating the current inventory index
void UInventoryComponent::EquipNewInventoryItem(AWeapon* NewItem)
{
	if (CurrentInventoryItem) {
		CurrentInventoryItem->Disable();
		CurrentInventoryItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	if (NewItem) {
		NewItem->Enable();
		ATestFireCharacter* OwningActor = Cast<ATestFireCharacter>(GetOwner());
		NewItem->AttachToComponent(OwningActor->GetSkeletalMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "GripPoint");

	}

	CurrentInventoryItem = NewItem;
}

// Determines if there is an inventory item to cycle to and set that item as the one to equip
void UInventoryComponent::SelectInventoryItem(EInventoryDirection Direction)
{
	if (Inventory.Num() == 0)
		return;

	AWeapon* InventoryItemToEquip = nullptr;
	int32 Index = (Direction == EInventoryDirection::NEXT ? 0 : Inventory.Num() - 1);

	if (CurrentInventoryItem) {
		Inventory.Find(CurrentInventoryItem, Index);
		Direction == EInventoryDirection::NEXT ? Index += 1 : Index -= 1;
	}

	if (Index < Inventory.Num() && Index > -1)
		InventoryItemToEquip = Inventory[Index];

	EquipNewInventoryItem(InventoryItemToEquip);
}
