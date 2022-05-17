// Fill out your copyright notice in the Description page of Project Settings.
// Test Fire is a simple 3D shooter created by Wilson Worlds, intended to build familiarity with the unreal engine and game design for 'Shooters'. June 22nd, 2021.

#include "Projectile.h"
#include "TestFire/Characters/TestFireCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default properties
	Damage = 10.0f;
	Speed = 4000.0f;
	LifeTime = 2.0f;
	MovementDirection = GetActorForwardVector() + GetActorLocation();
}

void AProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Subscribe to the hit event
	AActor::OnActorHit.AddDynamic(this, &AProjectile::OnActorHit);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// Destroy the projectile after a specific time
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AProjectile::DestroySelf, LifeTime, false);
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move the projectile in a forward direction
	FVector Location = GetActorLocation();
	FVector Direction = MovementDirection - Location;
	Location += Direction.GetSafeNormal() * Speed * DeltaTime;
	SetActorLocation(Location, true);
}

void AProjectile::OnActorHit(AActor* Self, AActor* Other, FVector NormalImpulse, const FHitResult& Hit)
{
	// Cause damage to the hit actor
	if (Other)
	{
		UGameplayStatics::ApplyDamage(Other, Damage, nullptr, this, DamageClass);
	}

	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation());
	}

	Destroy();
}

void AProjectile::SetMovementDirection(FVector dir)
{
	MovementDirection = dir;
}

void AProjectile::DestroySelf()
{
	Destroy();
}