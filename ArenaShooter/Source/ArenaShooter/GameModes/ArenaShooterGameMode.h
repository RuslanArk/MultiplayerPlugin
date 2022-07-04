// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ArenaShooterGameMode.generated.h"

class AArenaShooterCharacter;
class AArenaShooterPlayerController;

UCLASS()
class ARENASHOOTER_API AArenaShooterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PlayerEliminated(AArenaShooterCharacter* EliminatedCharacter, AArenaShooterPlayerController* VictimController, AArenaShooterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimedCharacter, AController* ElimedController);
	
};
