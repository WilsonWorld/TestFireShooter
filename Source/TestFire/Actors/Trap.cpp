// Fill out your copyright notice in the Description page of Project Settings.


#include "Trap.h"
#include "TestFire/Characters/TestFireCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"


ATrap::ATrap()
{
	PrimaryActorTick.bCanEverTick = true;

	// Additional Properties
	TrapDamage = 5.0f;
	CooldownTime = 1.0f;

	// Collision Box
	TrapCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapCollisionBox"));
	TrapCollisionBox->SetSimulatePhysics(false);
	TrapCollisionBox->SetCollisionProfileName("OverlapAllDynamic");
	TrapCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetRootComponent(TrapCollisionBox);

	TrapCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ATrap::BeginOverlap);
	TrapCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ATrap::EndOverlap);
}

void ATrap::CauseDamage()
{
	if (ActorToDamage)
	{
		UGameplayStatics::ApplyDamage(ActorToDamage, TrapDamage, GetWorld()->GetFirstPlayerController(), this->GetOwner(), DamageClass);
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Magenta, FString::Printf(TEXT("%s is on a Trap!"), *ActorToDamage->GetName()));
	}
}

void ATrap::BeginOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this)
	{
		ActorToDamage = Cast<ATestFireCharacter>(OtherActor);

		if (ActorToDamage)
		{
			UPrimitiveComponent* ComponentTrigger = Cast <UPrimitiveComponent>(ActorToDamage->GetRootComponent());

			if (OtherComp == ComponentTrigger)
			{
				GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ATrap::CauseDamage, CooldownTime, true, 0.5f);
			}
		}
	}
}

void ATrap::EndOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorldTimerManager().ClearTimer(CooldownTimerHandle);
}

// Called when the game starts or when spawned
void ATrap::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

