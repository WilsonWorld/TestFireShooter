// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TestFireCharacterAnimation.generated.h"


UCLASS()
class TESTFIRE_API UTestFireCharacterAnimation : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bHasWeapon;

protected:
	APawn* OwningPawn;

	UTestFireCharacterAnimation();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
};
