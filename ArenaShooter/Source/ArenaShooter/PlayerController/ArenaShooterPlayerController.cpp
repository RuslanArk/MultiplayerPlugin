// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaShooterPlayerController.h"

#include "ArenaShooter/Character/ArenaShooterCharacter.h"
#include "ArenaShooter/GameModes/ArenaShooterGameMode.h"
#include "ArenaShooter/HUD/Announcement.h"
#include "ArenaShooter/HUD/ArenaShooterHUD.h"
#include "ArenaShooter/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogASPlayerController, All, All);

void AArenaShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ArenaShooterHUD = Cast<AArenaShooterHUD>(GetHUD());
	if (ArenaShooterHUD)
	{
		ArenaShooterHUD->AddAnnouncement();
	}
}

void AArenaShooterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaShooterPlayerController, MatchState);
}

void AArenaShooterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHUDTime();
	CheckTimeSync(DeltaSeconds);
	PollInit();
}

void AArenaShooterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
		
	}
}

void AArenaShooterPlayerController::SetHUDScore(float Score)
{
	ArenaShooterHUD = ArenaShooterHUD == nullptr ? Cast<AArenaShooterHUD>(GetHUD()) : ArenaShooterHUD;

	const bool bHUDValid = ArenaShooterHUD &&
		ArenaShooterHUD->CharacterOverlay &&
		ArenaShooterHUD->CharacterOverlay->ScoreAmount;

	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		ArenaShooterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}
}

void AArenaShooterPlayerController::SetHUDDefeats(int32 Defeats)
{
	ArenaShooterHUD = ArenaShooterHUD == nullptr ? Cast<AArenaShooterHUD>(GetHUD()) : ArenaShooterHUD;

	const bool bHUDValid = ArenaShooterHUD &&
		ArenaShooterHUD->CharacterOverlay &&
		ArenaShooterHUD->CharacterOverlay->DefeatsAmount;

	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		ArenaShooterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}
}

void AArenaShooterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	ArenaShooterHUD = ArenaShooterHUD == nullptr ? Cast<AArenaShooterHUD>(GetHUD()) : ArenaShooterHUD;

	const bool bHUDValid = ArenaShooterHUD &&
		ArenaShooterHUD->CharacterOverlay &&
		ArenaShooterHUD->CharacterOverlay->WeaponAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		ArenaShooterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AArenaShooterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	ArenaShooterHUD = ArenaShooterHUD == nullptr ? Cast<AArenaShooterHUD>(GetHUD()) : ArenaShooterHUD;

	const bool bHUDValid = ArenaShooterHUD &&
		ArenaShooterHUD->CharacterOverlay &&
		ArenaShooterHUD->CharacterOverlay->CarriedAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		ArenaShooterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AArenaShooterPlayerController::SetHUDMatchCountDown(float CountDownTime)
{
	ArenaShooterHUD = ArenaShooterHUD == nullptr ? Cast<AArenaShooterHUD>(GetHUD()) : ArenaShooterHUD;

	const bool bHUDValid = ArenaShooterHUD &&
		ArenaShooterHUD->CharacterOverlay &&
		ArenaShooterHUD->CharacterOverlay->MatchCountDownText;

	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt(CountDownTime / 60.f);
		int32 Seconds = CountDownTime - Minutes * 60;
		
		FString CountDownText = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
		ArenaShooterHUD->CharacterOverlay->MatchCountDownText->SetText(FText::FromString(CountDownText));
	}
}

void AArenaShooterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AArenaShooterCharacter* ShooterCharacter = Cast<AArenaShooterCharacter>(InPawn))
	{
		SetHUDHealth(ShooterCharacter->GetHealth(), ShooterCharacter->GetMaxHealth());
	}	
}

void AArenaShooterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AArenaShooterPlayerController::SetHUDTime()
{
	uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());
	if (CountdownInt != SecondsLeft)
	{
		SetHUDMatchCountDown(MatchTime - GetServerTime());
	}
	
	CountdownInt = SecondsLeft;
}

void AArenaShooterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (ArenaShooterHUD && ArenaShooterHUD->CharacterOverlay)
		{
			CharacterOverlay = ArenaShooterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			}
		}
	}
}

void AArenaShooterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AArenaShooterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedClientRequest)
{
	float RoundServerTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundServerTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float AArenaShooterPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}

	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AArenaShooterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void AArenaShooterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void AArenaShooterPlayerController::HandleMatchHasStarted()
{
	ArenaShooterHUD = ArenaShooterHUD == nullptr ? Cast<AArenaShooterHUD>(GetHUD()) : ArenaShooterHUD;
	if (ArenaShooterHUD)
	{
		ArenaShooterHUD->AddCharacterOverlay();
		if (ArenaShooterHUD->Announcement)
		{
			ArenaShooterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
