// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingTarget.h"
#include "Components/BoxComponent.h"


AMovingTarget::AMovingTarget()
{
	PrimaryActorTick.bCanEverTick = true;

	InterpSpeed = 300.0f;
	StartingTarget = ETarget::First;

	// Target 1 Component defaults setup
	Target1 = CreateDefaultSubobject<UBoxComponent>("Target 1 Box");
	Target1->SetCollisionProfileName("NoCollision");
	Target1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Target1->SetupAttachment(RootComponent);

	// Target 2 Component defaults setup
	Target2 = CreateDefaultSubobject<UBoxComponent>("Target 2 Box");
	Target2->SetCollisionProfileName("NoCollision");
	Target2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Target2->SetupAttachment(RootComponent);
}

void AMovingTarget::BeginPlay()
{
	Super::BeginPlay();

	Target1Location = Target1->GetComponentLocation();
	Target2Location = Target2->GetComponentLocation();

	if (StartingTarget == ETarget::First)
	{
		CurrentLocationTarget = Target1Location;
	}
	else
	{
		CurrentLocationTarget = Target2Location;
	}
}

void AMovingTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Delta = TargetMesh->GetComponentLocation() - CurrentLocationTarget;
	float DistanceToPoint = Delta.Size();

	if (DistanceToPoint < 100)
	{
		if (CurrentLocationTarget != Target1Location)
		{
			CurrentLocationTarget = Target1Location;
		}
		else
		{
			CurrentLocationTarget = Target2Location;
		}
	}

	FVector targetLocation = FMath::VInterpConstantTo(TargetMesh->GetComponentLocation(), CurrentLocationTarget, DeltaTime, InterpSpeed);
	TargetMesh->SetWorldLocation(targetLocation);
}
