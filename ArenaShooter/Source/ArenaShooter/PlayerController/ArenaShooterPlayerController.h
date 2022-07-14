// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ArenaShooterPlayerController.generated.h"

class AArenaShooterHUD;

UCLASS()
class ARENASHOOTER_API AArenaShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY()
	AArenaShooterHUD* ArenaShooterHUD = nullptr;

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);

	virtual void OnPossess(APawn* InPawn) override;
	
protected:
	virtual void BeginPlay() override;
	
};
