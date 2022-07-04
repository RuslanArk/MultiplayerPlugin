// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaShooterGameMode.h"

#include "ArenaShooter/Character/ArenaShooterCharacter.h"
#include "ArenaShooter/PlayerController/ArenaShooterPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AArenaShooterGameMode::PlayerEliminated(AArenaShooterCharacter* EliminatedCharacter,
                                             AArenaShooterPlayerController* VictimController, AArenaShooterPlayerController* AttackerController)
{
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
