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

protected:

	float ClientServerDelta = 0.0f; // difference between client and server time
	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeSyncFrequency = 5.0f;
	float TimeSyncRunningTime = 0.0f;
	
private:
	UPROPERTY()
	AArenaShooterHUD* ArenaShooterHUD = nullptr;

	float MatchTime = 120.f;
	uint32 CountdownInt = 0;

public:
	virtual void Tick(float DeltaSeconds) override;
	
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountDown(float CountDownTime);

	virtual void OnPossess(APawn* InPawn) override;
	// synced with server clock as soon as possible
	virtual void ReceivedPlayer() override;

	virtual float GetServerTime(); // synced with server world clock
	
protected:
	virtual void BeginPlay() override;

	void SetHUDTime();

	/**
	 *	Sync time between client and server
	 */
	void CheckTimeSync(float DeltaTime);

	// Requests the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	//Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);
	
};
