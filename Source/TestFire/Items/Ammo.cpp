#include "Ammo.h"
#include "Kismet/GameplayStatics.h"

AAmmo::AAmmo()
{
	BulletCount = 30;
	Tags.Add("Ammo");
}

void AAmmo::Destroyed()
{
	Super::Destroyed();

	if (PickupSound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
}
