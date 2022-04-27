// Copyright Epic Games, Inc. All Rights Reserved.
// Test Fire is a simple 3D shooter created by Wilson Worlds, intended to build familiarity with the unreal engine and game design for 'Shooters'. June 22nd, 2021.

#include "TestFireCharacter.h"
#include "TestFire/Characters/TestFireCharacterAnimation.h"
#include "TestFire/Items/Item.h"
#include "TestFire/Items/Ammo.h"
#include "TestFire/Items/GrenadePickup.h"
#include "TestFire/Items/Weapon/Weapon.h"
#include "TestFire/Items/Explosives/Explosive.h"
#include "TestFire/Components/InventoryComponent.h"
#include "TestFire/Components/SlowTimeComponent.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
ATestFireCharacter::ATestFireCharacter() :
	SkeletalMesh(nullptr),
	AnimationInstance(nullptr)
{
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set default movement speed
	MaxWalkSpeed = 300.f;
	MaxJogSpeed = 450.0f;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = MaxJogSpeed;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a camera boom (pulls in towards the player if there is a collision)
	ShoulderCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("ShoulderCameraBoom"));
	ShoulderCameraBoom->SetupAttachment(GetCapsuleComponent());
	ShoulderCameraBoom->bUsePawnControlRotation = true;
	ShoulderCameraBoom->TargetArmLength = 75.0f;

	// Create a follow camera
	ShoulderCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ShoulderCamera"));
	ShoulderCamera->SetupAttachment(ShoulderCameraBoom, USpringArmComponent::SocketName);
	ShoulderCamera->bUsePawnControlRotation = false;

	// Create the 'Pickup Detection' Sphere Componenet
	PickupSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PickupTrigger"));
	PickupSphereComponent->SetupAttachment(RootComponent);
	PickupSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PickupSphereComponent->SetCollisionProfileName("OverlapAll");

	// Inventory
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("Inventory");

	// Slow Time
	SlowTimeComponent = CreateDefaultSubobject<USlowTimeComponent>("TimeControl");

	// Addition character properties
	CurrentWeapon = nullptr;

	MaxInteractDistance = 200.0f;
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	Score = 0;
	CurrentExplosivesCount = 0;
	MaxExplosivesCount = 5;

	CameraModeEnum = ECharacterCameraMode::ThirdPersonDefault;

	bIsFiring = false;
	bIsHolstered = false;

	// Trigger function to take damage when struck by a damage event causing actor
	OnTakeAnyDamage.AddDynamic(this, &ATestFireCharacter::TakeAnyDamage);

}

void ATestFireCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Retrieve the skeletal mesh
	SkeletalMesh = GetMesh();
	if (SkeletalMesh)
	{
		AnimationInstance = Cast<UTestFireCharacterAnimation>(SkeletalMesh->GetAnimInstance());
	}

	UpdateForCameraMode();
}

void ATestFireCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATestFireCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const bool bHasWeapon = CurrentWeapon != nullptr;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	if (CurrentHealth <= 0)
	{
		Destroy();
	}
}

void ATestFireCharacter::TakeAnyDamage(AActor* damagedActor, float Damage, const UDamageType* damageType, AController* InstigatedBy, AActor* damageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("taken %.2f damage"), Damage));

	CurrentHealth -= Damage;
}

void ATestFireCharacter::NextWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->UnEquip();
	}
	InventoryComponent->NextInventoryItem();

	CurrentWeapon = InventoryComponent->GetCurrentInventoryItem();

	if (CurrentWeapon)
	{
		CurrentWeapon->Equip(this);
	}
}

void ATestFireCharacter::PrevWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->UnEquip();
	}
	InventoryComponent->PrevInventoryItem();

	CurrentWeapon = InventoryComponent->GetCurrentInventoryItem();

	if (CurrentWeapon)
	{
		CurrentWeapon->Equip(this);
	}
}

void ATestFireCharacter::DropWeapon()
{
	/*
	if (CurrentWeapon)
	{
		CurrentWeapon->Drop();
	}
	*/
}

void ATestFireCharacter::ReloadWeapon()
{
	if (CurrentWeapon)
	{
		uint16 AmmoToAdd = CurrentWeapon->MaxAmmo - CurrentWeapon->CurrentAmmo;

		// Check ammo is being added before playing the sound effect
		if (AmmoToAdd != 0 && InventoryComponent->GetStoredRifleAmmo() != 0)
		{
			CurrentWeapon->PlayReloadEffects();
		}

		// Check if there is enough ammo for the reload in the player's inventory
		if (InventoryComponent->GetStoredRifleAmmo() >= AmmoToAdd)
		{
			InventoryComponent->RemoveAmmoFromRifleStorage(AmmoToAdd);
			CurrentWeapon->CurrentAmmo += AmmoToAdd;
		}
		// Check if there is still ammo that can be added from the player's inventory
		else if (InventoryComponent->GetStoredRifleAmmo() > 0 && InventoryComponent->GetStoredRifleAmmo() < AmmoToAdd)
		{
			uint16 RemainingAmmo = InventoryComponent->GetStoredRifleAmmo();

			InventoryComponent->RemoveAmmoFromRifleStorage(RemainingAmmo);
			CurrentWeapon->CurrentAmmo += RemainingAmmo;
		}
	}
}

void ATestFireCharacter::ThrowGrenade()
{
	if (CurrentExplosivesCount > 0)
	{
		// Spawn a gernade in front of the Player
		FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 100.0f) + (GetActorUpVector() * 50.0f);
		FRotator spawnRotation = GetControlRotation();
		AExplosive* spawnedGrenade = Cast<AExplosive>( GetWorld()->SpawnActor(ExplosiveToUse, &spawnLocation, &spawnRotation) );

		// Decrement the number of grenades the player has.
		SetGrenades(CurrentExplosivesCount - 1);
	}
}

void ATestFireCharacter::StartSlowTime()
{
	SlowTimeComponent->OnSlowTimeStart();
}

void ATestFireCharacter::StopSlowTime()
{
	SlowTimeComponent->OnSlowTimeEnd();
}

void ATestFireCharacter::CycleCamera()
{
	if (CurrentWeapon)
	{
		int newCameraMode = (int)CameraModeEnum + 1;

		if (newCameraMode >= ECharacterCameraMode::COUNT)
		{
			newCameraMode = ECharacterCameraMode::ThirdPersonDefault;
		}

		SetCameraMode((ECharacterCameraMode::Type)newCameraMode);
	}
}

void ATestFireCharacter::SetCameraMode(ECharacterCameraMode::Type newCameraMode)
{
	CameraModeEnum = newCameraMode;
	UpdateForCameraMode();
}

void ATestFireCharacter::UpdateForCameraMode()
{
	switch(CameraModeEnum)
	{
	case ECharacterCameraMode::ThirdPersonDefault:
		FollowCamera->SetActive(true);
		ShoulderCamera->SetActive(false);
		break;
	case ECharacterCameraMode::ThirdPersonOverShoulder:
		FollowCamera->SetActive(false);
		ShoulderCamera->SetActive(true);
		break;
	default:
		break;
	}

	APlayerController* OurPlayerControl = UGameplayStatics::GetPlayerController(this, 0);
	if (OurPlayerControl)
	{
		OurPlayerControl->SetViewTargetWithBlend(this, 0.25f, EViewTargetBlendFunction::VTBlend_EaseIn);
	}
}

AWeapon* ATestFireCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void ATestFireCharacter::SetGrenades(uint8 NumGrenades)
{
	CurrentExplosivesCount = NumGrenades;
}

void ATestFireCharacter::PullTrigger()
{
	if (CurrentWeapon)
	{
		// Grab the camera information and store in local variables
		FVector CamLoc;
		FRotator CamRot;
		Controller->GetPlayerViewPoint(CamLoc, CamRot);

		// Grab the player's rotation and change their Yaw to match the camera's yaw facing.
		FRotator PlayerRot = GetActorRotation();
		PlayerRot.Yaw = CamRot.Yaw;

		// Change the player's rotation to ensure that the fired projectile does not hit the player and aims the weapon in the direction the shots are fired
		SetActorRotation(PlayerRot);

		CurrentWeapon->OnTriggerPull();
		bIsFiring = true;
	}
}

void ATestFireCharacter::ReleaseTrigger()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->OnTriggerRelease();
		bIsFiring = false;
	}
}

void ATestFireCharacter::Interact()
{
	TArray<AActor*> overlappingActors;

	PickupSphereComponent->GetOverlappingActors(overlappingActors);

	for (int i = 0; i < overlappingActors.Num(); i++)
	{
		// If the actor has the Tag "Weapon"
		if (overlappingActors[i]->ActorHasTag("Weapon"))
		{
			AWeapon* Weapon = Cast<AWeapon>(overlappingActors[i]);
			if (Weapon)
			{
				InventoryComponent->AddToInventory(Weapon);
			}
		}

		if (overlappingActors[i]->ActorHasTag("Ammo"))
		{
			// Check if the Actor is an Ammo Pickup
			AAmmo* Ammo = Cast<AAmmo>(overlappingActors[i]);
			if (Ammo)
			{
				// Refill ammo to max capacity and detroy the pickup item
				InventoryComponent->AddAmmoToRifleStorage(Ammo->BulletCount);
				Ammo->Destroy();
			}
		}

		if (overlappingActors[i]->ActorHasTag("Grenade"))
		{
			//Check if the Actor is a Grenade Pickup
			AGrenadePickup* Grenade = Cast<AGrenadePickup>(overlappingActors[i]);
			if (Grenade)
			{
				if (CurrentExplosivesCount < MaxExplosivesCount)
				{
					CurrentExplosivesCount++;
					Grenade->Destroy();
				}
			}
		}
	}
}

AActor* ATestFireCharacter::RayCast_GetActor()
{
	if (Controller)
	{
		FVector PlayerLoc = this->GetActorLocation();
		FVector CamLoc;
		FRotator CamRot;
		Controller->GetPlayerViewPoint(CamLoc, CamRot);

		FVector StartTrace = PlayerLoc + FVector(0.0f, 0.0f, 50.0f);
		FVector Direction = CamRot.Vector();
		FVector EndTrace = StartTrace + Direction * MaxInteractDistance;

		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 2.0f, 0, 1.0f);

		FHitResult Hit(ForceInit);
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);

		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_WorldStatic, TraceParams);

		return Hit.GetActor();
	}
	else
	{
		return nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////
void ATestFireCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATestFireCharacter::PullTrigger);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATestFireCharacter::ReleaseTrigger);

	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &ATestFireCharacter::ThrowGrenade);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ATestFireCharacter::ReloadWeapon);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &ATestFireCharacter::NextWeapon);
	PlayerInputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ATestFireCharacter::PrevWeapon);

	PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &ATestFireCharacter::DropWeapon);

	PlayerInputComponent->BindAction("SlowTime", IE_Pressed, this, &ATestFireCharacter::StartSlowTime);
	PlayerInputComponent->BindAction("SlowTimeStop", IE_Pressed, this, &ATestFireCharacter::StopSlowTime);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ATestFireCharacter::Interact);

	InputComponent->BindAction("ToggleCamera", IE_Pressed, this, &ATestFireCharacter::CycleCamera);
	InputComponent->BindAction("ToggleCamera", IE_Released, this, &ATestFireCharacter::CycleCamera);

	// Default Unreal provided controls for a third person character
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATestFireCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATestFireCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATestFireCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATestFireCharacter::LookUpAtRate);
}

void ATestFireCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	MoveRight(Rate);
}

void ATestFireCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATestFireCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATestFireCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
