// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HudWidget.generated.h"


UCLASS()
class TESTFIRE_API UHudWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

public:
	void DisplayHealth();
	void DisplaySlowTime();
	void DisplayAmmo();
	void DisplayExplosives();
	void DisplayScore();
	class ATestFireCharacter* GetPlayer();
};
