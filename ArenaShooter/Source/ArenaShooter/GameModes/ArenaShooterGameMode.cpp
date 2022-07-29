// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaShooterGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

#include "ArenaShooter/Character/ArenaShooterCharacter.h"
#include "ArenaShooter/PlayerController/ArenaShooterPlayerController.h"
#include "ArenaShooter/PlayerState/ArenaShooterPlayerState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

AArenaShooterGameMode::AArenaShooterGameMode()
{
	bDelayedStart = true;
	
}

void AArenaShooterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AArenaShooterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetMatchState() == MatchState::WaitingToStart)
	{
		CountDownTime = WarmUpTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.0f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountDownTime = WarmUpTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
}

void AArenaShooterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AArenaShooterPlayerController* ArenaShooterPlayer = Cast<AArenaShooterPlayerController>(*It);
		if (ArenaShooterPlayer)
		{
			ArenaShooterPlayer->OnMatchStateSet(MatchState);
		}
	}
}

void AArenaShooterGameMode::PlayerEliminated(AArenaShooterCharacter* EliminatedCharacter,
                                             AArenaShooterPlayerController* VictimController, AArenaShooterPlayerController* AttackerController)
{
	AArenaShooterPlayerState* AttackerPlayerState = AttackerController ? Cast<AArenaShooterPlayerState>(AttackerController->PlayerState) : nullptr;
	AArenaShooterPlayerState* VictimPlayerState = VictimController ? Cast<AArenaShooterPlayerState>(VictimController->PlayerState) : nullptr;
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.0f);
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	
	if (EliminatedCharacter)
	{
		EliminatedCharacter->Elim();
	}
}

void AArenaShooterGameMode::RequestRespawn(ACharacter* ElimedCharacter, AController* ElimedController)
{
	if (ElimedCharacter)
	{
		ElimedCharacter->Reset();
		ElimedCharacter->Destroy();
	}

	if (ElimedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		const int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimedController, PlayerStarts[Selection]);
	}
}
