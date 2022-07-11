// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ArenaShooterPlayerState.generated.h"

class AArenaShooterCharacter;
class AArenaShooterPlayerController;

UCLASS()
class ARENASHOOTER_API AArenaShooterPlayerState : public APlayerState
{
	GENERATED_BODY()

	UPROPERTY()
	AArenaShooterCharacter* Character;
	UPROPERTY()
	AArenaShooterPlayerController* Controller;

public:
	virtual void OnRep_Score() override;

	void AddToScore(float ScoreAmount);
	
};
