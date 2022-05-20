#include "SlowTimeComponent.h"
#include "TestFire/TestFireController.h"
#include "Kismet/GameplayStatics.h"

USlowTimeComponent::USlowTimeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Time change properties
	CurrentSlowPoints = 0.0f;
	MaxSlowPoints = 5.0f;
	SlowPointChangeRate = 0.2f;
	bTimeIsSlow = false;
}

// Slow the passage of time for the world
void USlowTimeComponent::OnSlowTimeStart()
{
	if (CurrentSlowPoints > 0.0f) {
		APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
		GetWorld()->GetWorldSettings()->SetTimeDilation(0.25f);
		Controller->GetPawn()->CustomTimeDilation = 2.0f;
		bTimeIsSlow = true;

		ClearSlowTimer();
		GetWorld()->GetTimerManager().SetTimer(SlowTimeHandle, this, &USlowTimeComponent::DecreaseSlowPoints, SlowPointChangeRate, true, 0.2f);
	}

}

// Return the passage of time for the world to normal
void USlowTimeComponent::OnSlowTimeEnd()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f);
	Controller->GetPawn()->CustomTimeDilation = 1.0f;
	bTimeIsSlow = false;

	ClearSlowTimer();
	GetWorld()->GetTimerManager().SetTimer(SlowTimeHandle, this, &USlowTimeComponent::IncreaseSlowPoints, SlowPointChangeRate, true, 0.2f);
}

// Increase amount of time the Slow Time ability can run, up to the max stat.
void USlowTimeComponent::IncreaseSlowPoints()
{
	if (CurrentSlowPoints < MaxSlowPoints)
		CurrentSlowPoints += 0.05f;
	else
		CurrentSlowPoints = MaxSlowPoints;
}

// Decrease amount of time the Slow Time ability can run, up to the max stat.
void USlowTimeComponent::DecreaseSlowPoints()
{
	if (CurrentSlowPoints > 0.0f)
		CurrentSlowPoints -= 0.5f;
	else {
		CurrentSlowPoints = 0.0f;
		OnSlowTimeEnd();
	}
}

void USlowTimeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OnSlowTimeEnd();
}

void USlowTimeComponent::ClearSlowTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(SlowTimeHandle);
}

