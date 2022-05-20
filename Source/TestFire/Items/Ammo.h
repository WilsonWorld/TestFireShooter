#pragma once

#include "CoreMinimal.h"
#include "TestFire/Items/Item.h"
#include "Ammo.generated.h"


UCLASS()
class TESTFIRE_API AAmmo : public AItem
{
	GENERATED_BODY()
	
public:
	AAmmo();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		uint8 BulletCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
		class USoundBase* PickupSound;

	virtual void Destroyed() override;
};
