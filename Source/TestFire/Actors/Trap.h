// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trap.generated.h"

UCLASS()
class TESTFIRE_API ATrap : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
		class UBoxComponent* TrapCollisionBox;

	class ATestFireCharacter* ActorToDamage;

	FTimerHandle CooldownTimerHandle;
	float CooldownTime;

	UFUNCTION()
		void CauseDamage();

	ATrap();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
		TSubclassOf<UDamageType> DamageClass;

	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
		float TrapDamage;

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void EndOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
