// Test Fire is a simple 3D shooter created by Wilson Worlds, intended to build familiarity with the unreal engine and game design for 'Shooters'. June 22nd, 2021.

#include "Item.h"
#include "Components/SphereComponent.h"


AItem::AItem()
{
    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereCollision->SetSphereRadius(32.0f);
    SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SphereCollision->SetCollisionProfileName("OverlapAll");
    SphereCollision->SetGenerateOverlapEvents(true);
    SphereCollision->SetSimulatePhysics(false);
    SetRootComponent(SphereCollision);

	Tags.Empty();
	Tags.Add("Item");
}

void AItem::Disable()
{
	this->SetActorHiddenInGame(true);
    SphereCollision->SetActive(false);
    SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItem::Enable()
{
	this->SetActorHiddenInGame(false);
    SphereCollision->SetActive(true);
    SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
