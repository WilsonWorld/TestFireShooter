// Fill out your copyright notice in the Description page of Project Settings.
// Test Fire is a simple 3D shooter created by Wilson Worlds, intended to build familiarity with the unreal engine and game design for 'Shooters'. June 22nd, 2021.

#include "Projectile.h"

#include "Kismet/GameplayStatics.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default properties
	Damage = 10.0f;
	Speed = 4000.0f;
	LifeTime = 2.0f;
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

	FVector CamLoc;
	FRotator CamRot;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CamLoc, CamRot);

	// Move the projectile in a forward direction
	FVector Location = GetActorLocation();
	Location += CamRot.Vector() * Speed * DeltaTime;
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

void AProjectile::DestroySelf()
{
	Destroy();
}