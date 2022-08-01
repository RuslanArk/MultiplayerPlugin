// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaShooterPlayerController.h"

#include "ArenaShooter/Character/ArenaShooterCharacter.h"
#include "ArenaShooter/Components/CombatComponent.h"
#include "ArenaShooter/GameModes/ArenaShooterGameMode.h"
#include "ArenaShooter/HUD/Announcement.h"
#include "ArenaShooter/HUD/ArenaShooterHUD.h"
#include "ArenaShooter/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogASPlayerController, All, All);

void AArenaShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ArenaShooterHUD = Cast<AArenaShooterHUD>(GetHUD());
	ServerCheckMatchState();	
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

void AArenaShooterPlayerController::SetHUDMatchCountdown(float CountDownTime)
{
	ArenaShooterHUD = ArenaShooterHUD == nullptr ? Cast<AArenaShooterHUD>(GetHUD()) : ArenaShooterHUD;

	const bool bHUDValid = ArenaShooterHUD &&
		ArenaShooterHUD->CharacterOverlay &&
		ArenaShooterHUD->CharacterOverlay->MatchCountDownText;

	if (bHUDValid)
	{
		if (CountDownTime < 0.f)
		{
			ArenaShooterHUD->CharacterOverlay->MatchCountDownText->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountDownTime / 60.f);
		int32 Seconds = CountDownTime - Minutes * 60;
		
		FString CountdownText = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
		ArenaShooterHUD->CharacterOverlay->MatchCountDownText->SetText(FText::FromString(CountdownText));
	}
}

void AArenaShooterPlayerController::SetHUDAnnouncementCountdown(float CountDownTime)
{
	ArenaShooterHUD = ArenaShooterHUD == nullptr ? Cast<AArenaShooterHUD>(GetHUD()) : ArenaShooterHUD;

	const bool bHUDValid = ArenaShooterHUD &&
		ArenaShooterHUD->Announcement &&
		ArenaShooterHUD->Announcement->WarmupTime;

	if (bHUDValid)
	{
		if (CountDownTime < 0.f)
		{
			ArenaShooterHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountDownTime / 60.f);
		int32 Seconds = CountDownTime - Minutes * 60;
		
		FString CountDownText = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
		ArenaShooterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountDownText));
	}
}

void AArenaShooterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.0f;
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::Cooldown)
	{
		TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		else if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	
	CountdownInt = SecondsLeft;
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
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AArenaShooterPlayerController::ServerCheckMatchState_Implementation()
{
	AArenaShooterGameMode* GameMode = Cast<AArenaShooterGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmUpTime;
		MatchTime = GameMode->MatchTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		CooldownTime = GameMode->CooldownTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, LevelStartingTime, CooldownTime);
	}
}

void AArenaShooterPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	CooldownTime = Cooldown;
	OnMatchStateSet(MatchState);	
	if (ArenaShooterHUD && MatchState == MatchState::WaitingToStart)
	{
		ArenaShooterHUD->AddAnnouncement();
	}
}

void AArenaShooterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
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

void AArenaShooterPlayerController::HandleCooldown()
{
	ArenaShooterHUD = ArenaShooterHUD == nullptr ? Cast<AArenaShooterHUD>(GetHUD()) : ArenaShooterHUD;
	if (ArenaShooterHUD && ArenaShooterHUD->CharacterOverlay)
	{
		ArenaShooterHUD->CharacterOverlay->RemoveFromParent();
		bool BHUDValid = ArenaShooterHUD->Announcement
			&& ArenaShooterHUD->Announcement->AnnouncementText
			&& ArenaShooterHUD->Announcement->InfoText;
		if (BHUDValid)
		{
			ArenaShooterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In : ");
			ArenaShooterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
			ArenaShooterHUD->Announcement->InfoText->SetText(FText());
		}
	}

	AArenaShooterCharacter* ArenaShooterCharacter = Cast<AArenaShooterCharacter>(GetPawn());
	if (ArenaShooterCharacter && ArenaShooterCharacter->GetCombatComponent())
	{
		ArenaShooterCharacter->bDisableGameplay = true;
		ArenaShooterCharacter->GetCombatComponent()->FireButtonPressed(false);
	}
}