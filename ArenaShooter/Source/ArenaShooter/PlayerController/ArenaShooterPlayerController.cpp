// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaShooterPlayerController.h"

#include "ArenaShooter/HUD/ArenaShooterHUD.h"
#include "ArenaShooter/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void AArenaShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ArenaShooterHUD = Cast<AArenaShooterHUD>(GetHUD());
}

void AArenaShooterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	ArenaShooterHUD = ArenaShooterHUD == nullptr ? Cast<AArenaShooterHUD>(GetHUD()) : ArenaShooterHUD;

	const bool bHUDValid = ArenaShooterHUD &&
		ArenaShooterHUD->CharacterOverlay &&
		ArenaShooterHUD->CharacterOverlay->HealthBar &&
		ArenaShooterHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		ArenaShooterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		ArenaShooterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}