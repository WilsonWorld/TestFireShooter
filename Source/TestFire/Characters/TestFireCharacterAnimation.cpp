// Fill out your copyright notice in the Description page of Project Settings.

#include "TestFireCharacterAnimation.h"
#include "TestFire/TestFire.h"

UTestFireCharacterAnimation::UTestFireCharacterAnimation() :
	bHasWeapon(false),
	OwningPawn(nullptr)
{
}

void UTestFireCharacterAnimation::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningPawn = TryGetPawnOwner();
}

void UTestFireCharacterAnimation::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
}
