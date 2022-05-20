// Copyright Epic Games, Inc. All Rights Reserved.
// Test Fire is a simple 3D shooter created by Wilson World Games, intended to build familiarity with the unreal engine and game design for 'Shooters'. June 22nd, 2021.

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
	GetCharacterMovement()->JumpZVelocity = 600.f;
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

	// Custom Compoents
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("Inventory");
	SlowTimeComponent = CreateDefaultSubobject<USlowTimeComponent>("TimeControl");

	// Addition character properties
	CurrentWeapon = nullptr;
	MaxInteractDistance = 200.0f;
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	ReloadTime = 2.3f;
	Score = 0;
	CurrentExplosivesCount = 0;
	MaxExplosivesCount = 5;
	CameraModeEnum = ECharacterCameraMode::ThirdPersonDefault;
	bIsFiring = false;
	bIsReloading = false;

	// Trigger function to take damage when struck by a damage event causing actor
	OnTakeAnyDamage.AddDynamic(this, &ATestFireCharacter::TakeAnyDamage);
}

void ATestFireCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SkeletalMesh = GetMesh();
	if (SkeletalMesh) {
		AnimationInstance = Cast<UTestFireCharacterAnimation>(SkeletalMesh->GetAnimInstance());
	}

	UpdateForCameraMode();
}

void ATestFireCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const bool bHasWeapon = CurrentWeapon != nullptr;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ATestFireCharacter::TakeAnyDamage(AActor* damagedActor, float Damage, const UDamageType* damageType, AController* InstigatedBy, AActor* damageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("taken %.2f damage"), Damage));
	CurrentHealth -= Damage;

	if (CurrentHealth <= 0) {
		if (CurrentWeapon)
			CurrentWeapon->UnEquip();

		FTimerHandle DestroyTimer;
		float DestroyTime = 1.0f;
		GetWorldTimerManager().SetTimer(DestroyTimer, this, &ATestFireCharacter::DestroySelf, DestroyTime, false, 0.0f);
	}
}

// Starts firing the weapon
void ATestFireCharacter::PullTrigger()
{
	if (CurrentWeapon == nullptr || bIsReloading == true)
		return;

	FVector CamLoc;
	FRotator CamRot;
	Controller->GetPlayerViewPoint(CamLoc, CamRot);

	// Grab the player's rotation and change their Yaw to match the camera's yaw facing.
	FRotator PlayerRot = GetActorRotation();
	PlayerRot.Yaw = CamRot.Yaw;
	SetActorRotation(PlayerRot);

	CurrentWeapon->OnTriggerPull();
	bIsFiring = true;
}

// Stops the weapon from firing
void ATestFireCharacter::ReleaseTrigger()
{
	if (CurrentWeapon == nullptr)
		return;

	CurrentWeapon->OnTriggerRelease();
	bIsFiring = false;
}

// Check if the weapon can be reloaded, then play reload effects before starting the reload timer
void ATestFireCharacter::StartReload()
{
	if (CurrentWeapon == nullptr || bIsReloading == true || CurrentWeapon->CurrentAmmo == CurrentWeapon->MaxAmmo || InventoryComponent->GetStoredRifleAmmo() == 0)
		return;

	bIsReloading = true;
	CurrentWeapon->PlayReloadEffects();
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &ATestFireCharacter::ReloadWeapon, ReloadTime, false);
}

// Grab ammo from the inventory storage to refill the current weapon back to max, if possible. Reset reload timer.
void ATestFireCharacter::ReloadWeapon()
{
	if (CurrentWeapon == nullptr)
		return;

	uint16 AmmoToAdd = CurrentWeapon->MaxAmmo - CurrentWeapon->CurrentAmmo;

	// Check if there is enough ammo for the reload in the player's inventory
	if (InventoryComponent->GetStoredRifleAmmo() >= AmmoToAdd) {
		InventoryComponent->RemoveAmmoFromRifleStorage(AmmoToAdd);
		CurrentWeapon->CurrentAmmo += AmmoToAdd;
	}
	// Check if there is still ammo that can be added from the player's inventory
	else if (InventoryComponent->GetStoredRifleAmmo() > 0 && InventoryComponent->GetStoredRifleAmmo() < AmmoToAdd) {
		uint16 RemainingAmmo = InventoryComponent->GetStoredRifleAmmo();
		InventoryComponent->RemoveAmmoFromRifleStorage(RemainingAmmo);
		CurrentWeapon->CurrentAmmo += RemainingAmmo;
	}

	ClearReloadTimer();
}

// Reset the Reload Timer and set reloading as over
void ATestFireCharacter::ClearReloadTimer()
{
	GetWorldTimerManager().ClearTimer(ReloadTimer);
	bIsReloading = false;
}

// Cycles to the next inventory item if the player has a weapon
void ATestFireCharacter::NextWeapon()
{
	if (CurrentWeapon)
		CurrentWeapon->UnEquip();

	InventoryComponent->NextInventoryItem();
	CurrentWeapon = InventoryComponent->GetCurrentInventoryItem();

	if (CurrentWeapon)
		CurrentWeapon->Equip(this);
}

// Cycles to the previous inventory item if the player has a weapon
void ATestFireCharacter::PrevWeapon()
{
	if (CurrentWeapon)
		CurrentWeapon->UnEquip();

	InventoryComponent->PrevInventoryItem();
	CurrentWeapon = InventoryComponent->GetCurrentInventoryItem();

	if (CurrentWeapon)
		CurrentWeapon->Equip(this);
}

// Spawns in a grenade and reduce the player's grenade count
void ATestFireCharacter::ThrowGrenade()
{
	if (CurrentExplosivesCount > 0) {
		// Spawn a gernade in front of the Player
		FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 100.0f) + (GetActorUpVector() * 50.0f);
		FRotator spawnRotation = GetControlRotation();
		AExplosive* spawnedGrenade = Cast<AExplosive>( GetWorld()->SpawnActor(ExplosiveToUse, &spawnLocation, &spawnRotation) );

		// Decrement the number of grenades the player has.
		SetGrenades(CurrentExplosivesCount - 1);
	}
}

// Increments the camera mode, resetting when hitting mode max
void ATestFireCharacter::CycleCamera()
{
	if (CurrentWeapon == nullptr)
		return;

	int newCameraMode = (int)CameraModeEnum + 1;

	if (newCameraMode >= ECharacterCameraMode::COUNT) {
		newCameraMode = ECharacterCameraMode::ThirdPersonDefault;
	}

	SetCameraMode((ECharacterCameraMode::Type)newCameraMode);
}

// Change cameras based on enum and updates the viewport
void ATestFireCharacter::SetCameraMode(ECharacterCameraMode::Type newCameraMode)
{
	CameraModeEnum = newCameraMode;
	UpdateForCameraMode();
}

// Changes the camera based on current camera mode
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
	if (OurPlayerControl) {
		OurPlayerControl->SetViewTargetWithBlend(this, 0.25f, EViewTargetBlendFunction::VTBlend_EaseIn);
	}
}

void ATestFireCharacter::Interact()
{
	// Grab all the actors with the pick up sphere's radius
	TArray<AActor*> overlappingActors;
	PickupSphereComponent->GetOverlappingActors(overlappingActors);

	// If the actor has the Tag "Weapon", cast actor as weapon and add it to the inventory
	for (int i = 0; i < overlappingActors.Num(); i++) {
		if (overlappingActors[i]->ActorHasTag("Weapon")) {
			AWeapon* Weapon = Cast<AWeapon>(overlappingActors[i]);
			if (Weapon == nullptr)
				return;

			InventoryComponent->AddToInventory(Weapon);
		}

		// Check if the Actor is an "Ammo" Pickup, refill ammo to max capacity and detroy the pickup item
		if (overlappingActors[i]->ActorHasTag("Ammo")) {
			AAmmo* Ammo = Cast<AAmmo>(overlappingActors[i]);
			if (Ammo == nullptr)
				return;

			InventoryComponent->AddAmmoToRifleStorage(Ammo->BulletCount);
			Ammo->Destroy();
		}

		//Check if the Actor is a "Grenade" Pickup, increment grenade count if not at max already and destroys the pickup item
		if (overlappingActors[i]->ActorHasTag("Grenade")) {
			AGrenadePickup* Grenade = Cast<AGrenadePickup>(overlappingActors[i]);
			if (Grenade == nullptr)
				return;

				if (CurrentExplosivesCount < MaxExplosivesCount){
					CurrentExplosivesCount++;
					Grenade->Destroy();
				}
		}
	}
}

void ATestFireCharacter::DestroySelf()
{
	FTimerHandle DestroyTimer;
	float DestroyTime = 1.0f;
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &ATestFireCharacter::DestroySelf, DestroyTime, false, 0.0f);
	Destroy();
}

// Quickly ends the game for the user 
void ATestFireCharacter::EndAndExitGame()
{
	APlayerController* OurPlayerControl = UGameplayStatics::GetPlayerController(this, 0);
	UKismetSystemLibrary::QuitGame(this, OurPlayerControl, EQuitPreference::Quit, true);
}

AWeapon* ATestFireCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void ATestFireCharacter::SetGrenades(uint8 NumGrenades)
{
	CurrentExplosivesCount = NumGrenades;
}

void ATestFireCharacter::StartSlowTime()
{
	SlowTimeComponent->OnSlowTimeStart();
}

void ATestFireCharacter::StopSlowTime()
{
	SlowTimeComponent->OnSlowTimeEnd();
}

void ATestFireCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATestFireCharacter::PullTrigger);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATestFireCharacter::ReleaseTrigger);

	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &ATestFireCharacter::ThrowGrenade);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ATestFireCharacter::StartReload);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &ATestFireCharacter::NextWeapon);
	PlayerInputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ATestFireCharacter::PrevWeapon);

	PlayerInputComponent->BindAction("SlowTime", IE_Pressed, this, &ATestFireCharacter::StartSlowTime);
	PlayerInputComponent->BindAction("SlowTimeStop", IE_Pressed, this, &ATestFireCharacter::StopSlowTime);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ATestFireCharacter::Interact);

	InputComponent->BindAction("ToggleCamera", IE_Pressed, this, &ATestFireCharacter::CycleCamera);
	InputComponent->BindAction("ToggleCamera", IE_Released, this, &ATestFireCharacter::CycleCamera);

	PlayerInputComponent->BindAction("QuitGame", IE_Pressed, this, &ATestFireCharacter::EndAndExitGame);

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
