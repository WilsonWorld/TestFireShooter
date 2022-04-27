// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowTimeComponent.h"
#include "TestFire/TestFireController.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
USlowTimeComponent::USlowTimeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Time change properties
	CurrentSlowPoints = 0.0f;
	MaxSlowPoints = 5.0f;
	SlowPointChangeRate = 0.2f;
	bTimeIsSlow = false;
}

void USlowTimeComponent::OnSlowTimeStart()
{
	if (CurrentSlowPoints > 0.0f)
	{
		// Slow the passage of time for the world
		APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
		GetWorld()->GetWorldSettings()->SetTimeDilation(0.25f);
		Controller->GetPawn()->CustomTimeDilation = 2.0f;
		bTimeIsSlow = true;

		ClearSlowTimer();
		GetWorld()->GetTimerManager().SetTimer(SlowTimeHandle, this, &USlowTimeComponent::DecreaseSlowPoints, SlowPointChangeRate, true, 0.2f);
	}

}

void USlowTimeComponent::OnSlowTimeEnd()
{
	// Return the passage of time for the world to normal
	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f);
	Controller->GetPawn()->CustomTimeDilation = 1.0f;
	bTimeIsSlow = false;

	ClearSlowTimer();
	GetWorld()->GetTimerManager().SetTimer(SlowTimeHandle, this, &USlowTimeComponent::IncreaseSlowPoints, SlowPointChangeRate, true, 0.2f);
}

void USlowTimeComponent::IncreaseSlowPoints()
{
	if (CurrentSlowPoints < MaxSlowPoints)
	{
		CurrentSlowPoints += 0.05f;
	}
	else
	{
		CurrentSlowPoints = MaxSlowPoints;
	}
}

void USlowTimeComponent::DecreaseSlowPoints()
{
	if (CurrentSlowPoints > 0.0f)
	{
		CurrentSlowPoints -= 0.5f;
	}
	else
	{
		CurrentSlowPoints = 0.0f;
		OnSlowTimeEnd();
	}
}


// Called when the game starts
void USlowTimeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OnSlowTimeEnd();
}


// Called every frame
void USlowTimeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

}

void USlowTimeComponent::ClearSlowTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(SlowTimeHandle);
}

