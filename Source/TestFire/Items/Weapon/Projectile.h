// Fill out your copyright notice in the Description page of Project Settings.
// Test Fire is a simple 3D shooter created by Wilson Worlds, intended to build familiarity with the unreal engine and game design for 'Shooters'. June 22nd, 2021.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class TESTFIRE_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditDefaultsOnly)
		float Damage;

	UPROPERTY(EditDefaultsOnly)
		float Speed;

	UPROPERTY(EditDefaultsOnly)
		float LifeTime;

	UPROPERTY(EditDefaultsOnly)
		FVector MovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
		UParticleSystem* ImpactFX;

	void SetMovementDirection(FVector dir);

protected:
	UPROPERTY(EditDefaultsOnly, Category = Damage)
		TSubclassOf<UDamageType> DamageClass;

	// Sets default values for this actor's properties
	AProjectile();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnActorHit(AActor* Self, AActor* Other, FVector NormalImpulse, const FHitResult& Hit);

private:
	FTimerHandle Timer;

	void DestroySelf();
};
