// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ArenaShooterGameState.generated.h"

class AArenaShooterPlayerState;

UCLASS()
class ARENASHOOTER_API AArenaShooterGameState : public AGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	TArray<AArenaShooterPlayerState*> TopScoringPlayers;

private:
	float TopScore = 0.f;

public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void UpdateTopScore(AArenaShooterPlayerState* ScoringPlayer);
	
};
