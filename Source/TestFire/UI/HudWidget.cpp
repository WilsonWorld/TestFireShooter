#include "HudWidget.h"
#include "TestFire/Characters/TestFireCharacter.h"
#include "TestFire/Items/Weapon/Weapon.h"
#include "TestFire/Components/InventoryComponent.h"
#include "TestFire/Components/SlowTimeComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/GamePlayStatics.h"

bool UHudWidget::Initialize()
{
    bool bResult = Super::Initialize();
    if (!bResult)
        return false;

    UTextBlock* AmmoWidget = Cast<UTextBlock>(GetWidgetFromName("CurrentAmmoCount"));
    if (AmmoWidget)
        AmmoWidget->SetText(FText::FromString("Ammo: 0 / 0"));

    UTextBlock* ExplosivesWidget = Cast<UTextBlock>(GetWidgetFromName("CurrentExplosivesCount"));
    if (ExplosivesWidget)
        ExplosivesWidget->SetText(FText::FromString("Grenades: 0"));

    UTextBlock* ScoreWidget = Cast<UTextBlock>(GetWidgetFromName("CurrentScoreCount"));
    if (ScoreWidget)
        ScoreWidget->SetText(FText::FromString("Score: 0"));

    return true;
}

void UHudWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);

    DisplayHealth();
    DisplaySlowTime();
    DisplayAmmo();
    DisplayExplosives();
    DisplayScore();
}

void UHudWidget::DisplayHealth()
{
    ATestFireCharacter* pPlayer = GetPlayer();

    if (pPlayer) {
        float HealthPercent = pPlayer->CurrentHealth / pPlayer->MaxHealth;
        UProgressBar* Widget = Cast <UProgressBar>(GetWidgetFromName("HealthBar"));

        if (Widget)
            Widget->SetPercent(HealthPercent);
    }
}

void UHudWidget::DisplaySlowTime()
{
    ATestFireCharacter* pPlayer = GetPlayer();

    if (pPlayer) {
        float SlowPercent = pPlayer->GetSlowTimeComponent()->CurrentSlowPoints / pPlayer->GetSlowTimeComponent()->MaxSlowPoints;
        UProgressBar* Widget = Cast <UProgressBar>(GetWidgetFromName("SlowTimeBar"));

        if (Widget)
            Widget->SetPercent(SlowPercent);
    }
}

void UHudWidget::DisplayAmmo()
{
    ATestFireCharacter* pPlayer = GetPlayer();
    FString value;

    if (pPlayer) {
        if (pPlayer->GetCurrentWeapon() != nullptr)
            value = FString::Printf(TEXT("Ammo: %d / %d"), pPlayer->GetCurrentWeapon()->GetCurrentAmmo(), pPlayer->GetInventoryComponent()->GetStoredRifleAmmo());
        else
            value = FString::Printf(TEXT(""));

        UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("CurrentAmmoCount"));

        if (Widget)
            Widget->SetText(FText::FromString(value));
    }
}

void UHudWidget::DisplayExplosives()
{
    ATestFireCharacter* pPlayer = GetPlayer();
    FString value;

    if (pPlayer) {
        if (pPlayer->CurrentExplosivesCount != 0)
            value = FString::Printf(TEXT("Grenades: %d"), pPlayer->CurrentExplosivesCount);
        else
            value = FString::Printf(TEXT(""));

        UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("CurrentExplosivesCount"));

        if (Widget)
            Widget->SetText(FText::FromString(value));
    }
}

void UHudWidget::DisplayScore()
{
    ATestFireCharacter* pPlayer = GetPlayer();
    FString value;

    if (pPlayer) {
        value = FString::Printf(TEXT("Score: %d"), pPlayer->Score);
        UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("CurrentScoreCount"));

        if (Widget)
            Widget->SetText(FText::FromString(value));
    }
}

ATestFireCharacter* UHudWidget::GetPlayer()
{
    AController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    APawn* pPlayerPawn = pController->GetPawn();
    ATestFireCharacter* pPlayer = Cast<ATestFireCharacter>(pPlayerPawn);
    return pPlayer;
}
