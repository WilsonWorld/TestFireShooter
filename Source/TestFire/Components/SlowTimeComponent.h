// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SlowTimeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTFIRE_API USlowTimeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditDefaultsOnly)
		float CurrentSlowPoints;

	UPROPERTY(EditDefaultsOnly)
		float MaxSlowPoints;

	UPROPERTY(EditDefaultsOnly)
		float SlowPointChangeRate;

	// Sets default values for this component's properties
	USlowTimeComponent();

	UFUNCTION()
		void OnSlowTimeStart();

	UFUNCTION()
		void OnSlowTimeEnd();

protected:
	void IncreaseSlowPoints();
	void DecreaseSlowPoints();

	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FTimerHandle SlowTimeHandle;
	bool bTimeIsSlow;

	void ClearSlowTimer();
};
