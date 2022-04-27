// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestFireController.generated.h"

/**
 * 
 */
UCLASS()
class TESTFIRE_API ATestFireController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	
public:
	UPROPERTY()
		class UHudWidget* GameInfoWidget;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UHudWidget> GameInfoBP;
};
