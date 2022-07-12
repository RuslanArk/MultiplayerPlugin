// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaShooterPlayerState.h"

#include "Net/UnrealNetwork.h"

#include "ArenaShooter/Character/ArenaShooterCharacter.h"
#include "ArenaShooter/PlayerController/ArenaShooterPlayerController.h"


void AArenaShooterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaShooterPlayerState, Defeats);
}

void AArenaShooterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<AArenaShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AArenaShooterPlayerController>(Character->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void AArenaShooterPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<AArenaShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AArenaShooterPlayerController>(Character->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void AArenaShooterPlayerState::AddToScore(float ScoreAmount)
{
	float CurrentScore = GetScore();
	CurrentScore += ScoreAmount;
	Character = Character == nullptr ? Cast<AArenaShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AArenaShooterPlayerController>(Character->GetController()) : Controller;
		if (Controller)
		{
			SetScore(CurrentScore);
			Controller->SetHUDScore(CurrentScore);
		}
	}
}

void AArenaShooterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<AArenaShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AArenaShooterPlayerController>(Character->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

