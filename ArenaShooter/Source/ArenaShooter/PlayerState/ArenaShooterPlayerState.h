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
	AArenaShooterCharacter* Character = nullptr;
	UPROPERTY()
	AArenaShooterPlayerController* Controller = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats = 0;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_Defeats();

	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

	
};
