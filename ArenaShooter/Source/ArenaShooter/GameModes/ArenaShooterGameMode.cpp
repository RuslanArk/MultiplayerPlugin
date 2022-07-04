// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaShooterGameMode.h"

#include "ArenaShooter/Character/ArenaShooterCharacter.h"
#include "ArenaShooter/PlayerController/ArenaShooterPlayerController.h"

void AArenaShooterGameMode::PlayerEliminated(AArenaShooterCharacter* EliminatedCharacter,
	AArenaShooterPlayerController* VictimController, AArenaShooterPlayerController* AttackerController)
{
	if (EliminatedCharacter)
	{
		EliminatedCharacter->Elim();
		
	}
}
