// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ArenaShooterPlayerController.generated.h"

class AArenaShooterHUD;
class UCharacterOverlay;

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

	float LevelStartingTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UPROPERTY()
	UCharacterOverlay* CharacterOverlay;

	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;

public:
	virtual void Tick(float DeltaSeconds) override;
	
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountDown(float CountDownTime);
	void SetHUDAnnouncementCountDown(float CountDownTime);

	virtual void OnPossess(APawn* InPawn) override;
	// synced with server clock as soon as possible
	virtual void ReceivedPlayer() override;

	virtual float GetServerTime(); // synced with server world clock

	void OnMatchStateSet(FName State);

	void HandleCooldown();
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetHUDTime();
	void PollInit();

	void HandleMatchHasStarted();

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
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();
	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float StartingTime);

private:
	
	UFUNCTION()
	void OnRep_MatchState();
};
