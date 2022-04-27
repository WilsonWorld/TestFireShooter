// Fill out your copyright notice in the Description page of Project Settings.


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
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	m_MaxStoredAmmo = 300;
	m_StoredPistolAmmo = 0;
	m_StoredRifleAmmo = 0;
}

void UInventoryComponent::AddToInventory(AWeapon* pItem)
{
	Inventory.AddUnique(pItem);

	//Disable the Item
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

void UInventoryComponent::AddAmmoToPistolStorage(uint16 Ammo)
{
	m_StoredPistolAmmo += Ammo;

	if (GetStoredPistolAmmo() > GetMaxAmmoStorage())
	{
		m_StoredPistolAmmo = m_MaxStoredAmmo;
	}
}

void UInventoryComponent::RemoveAmmoFromPistolStorage(uint16 Ammo)
{
	m_StoredPistolAmmo -= Ammo;

	if (GetStoredPistolAmmo() <= 0)
	{
		m_StoredPistolAmmo = 0;
	}
}

void UInventoryComponent::AddAmmoToRifleStorage(uint16 Ammo)
{
	m_StoredRifleAmmo += Ammo;

	if (GetStoredRifleAmmo() > GetMaxAmmoStorage())
	{
		m_StoredRifleAmmo = m_MaxStoredAmmo;
	}
}

void UInventoryComponent::RemoveAmmoFromRifleStorage(uint16 Ammo)
{
	m_StoredRifleAmmo -= Ammo;

	if (GetStoredRifleAmmo() <= 0)
	{
		m_StoredPistolAmmo = 0;
	}
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


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UInventoryComponent::EquipNewInventoryItem(AWeapon* NewItem)
{
	//IF CurrentInventoryItem IS NOT nullptr
	if (CurrentInventoryItem)
	{
		//CALL Disable() on CurrentInventoryItem
		CurrentInventoryItem->Disable();

		CurrentInventoryItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	}
	//ENDIF

	//IF NewItem IS NOT nullptr
	if (NewItem)
	{
		//CALL Enable() on NewItem
		NewItem->Enable();

		ATestFireCharacter* OwningActor = Cast<ATestFireCharacter>(GetOwner());
		NewItem->AttachToComponent(OwningActor->GetSkeletalMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "GripPoint");

	}
	//ENDIF

	//SET CurrentInventoryItem to NewItem
	CurrentInventoryItem = NewItem;
}

void UInventoryComponent::SelectInventoryItem(EInventoryDirection Direction)
{
	if (Inventory.Num() == 0)
	{
		return;
	}

	AWeapon* InventoryItemToEquip = nullptr;

	int32 Index = (Direction == EInventoryDirection::NEXT ? 0 : Inventory.Num() - 1);

	if (CurrentInventoryItem)
	{
		Inventory.Find(CurrentInventoryItem, Index);

		Direction == EInventoryDirection::NEXT ? Index += 1 : Index -= 1;
	}

	if (Index < Inventory.Num() && Index > -1)
	{
		InventoryItemToEquip = Inventory[Index];
	}

	EquipNewInventoryItem(InventoryItemToEquip);
}
