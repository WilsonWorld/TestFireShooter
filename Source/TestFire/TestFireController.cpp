// Fill out your copyright notice in the Description page of Project Settings.


#include "TestFireController.h"
#include "TestFire/UI/HudWidget.h"


void ATestFireController::BeginPlay()
{
	Super::BeginPlay();

	if (GameInfoBP != nullptr && IsLocalController())
	{
		GameInfoWidget = CreateWidget<UHudWidget>(this, GameInfoBP);
		GameInfoWidget->AddToViewport();
	}
}
