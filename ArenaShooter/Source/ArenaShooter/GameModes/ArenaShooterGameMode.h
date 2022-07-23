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
	UPROPERTY(EditDefaultsOnly)
	float WarmUpTime = 10.f;
	UPROPERTY(EditDefaultsOnly)
	float LevelStartingTime = 0.f;

private:
	float CountDownTime = 0.f;

public:
	AArenaShooterGameMode();

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void PlayerEliminated(AArenaShooterCharacter* EliminatedCharacter, AArenaShooterPlayerController* VictimController, AArenaShooterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimedCharacter, AController* ElimedController);

protected:
	virtual void BeginPlay() override;
};

