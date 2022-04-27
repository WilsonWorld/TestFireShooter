// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"

#include "Kismet/GameplayStatics.h"

AAmmo::AAmmo()
{
	PrimaryActorTick.bCanEverTick = false;

	BulletCount = 30;

	Tags.Add("Ammo");
}

void AAmmo::Destroyed()
{
	Super::Destroyed();

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
	}
}
