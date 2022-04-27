// Fill out your copyright notice in the Description page of Project Settings.


#include "PracticeTarget.h"
#include "TestFire/Characters/TestFireCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GamePlayStatics.h"

// Sets default values
APracticeTarget::APracticeTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = 100.0f;
	PointValue = 100;

	// Mesh
	TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>("TargetMesh");
	SetRootComponent(TargetMesh);

	// Trigger function to take damage when struck by a damage event causing actor
	OnTakeAnyDamage.AddDynamic(this, &APracticeTarget::TakeAnyDamage);
}

void APracticeTarget::IncreasePlayerScore()
{
	AController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn* pPlayerPawn = pController->GetPawn();
	ATestFireCharacter* pPlayer = Cast<ATestFireCharacter>(pPlayerPawn);

	if (pPlayer)
	{
		pPlayer->Score += PointValue;
	}
}

void APracticeTarget::TakeAnyDamage(AActor* damagedActor, float Damage, const UDamageType* damageType, AController* InstigatedBy, AActor* damageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("taken %.2f damage"), Damage));

	Health -= Damage;

	if (Health <= 0)
	{
		if (BreakingSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, BreakingSound, GetActorLocation());
		}
		IncreasePlayerScore();
		Destroy();
	}
}
