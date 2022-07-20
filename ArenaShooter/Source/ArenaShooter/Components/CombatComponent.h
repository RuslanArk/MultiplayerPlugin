// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArenaShooter/HUD/ArenaShooterHUD.h"
#include "ArenaShooter/Weapon/WeaponTypes.h"
#include "ArenaShooter/ArenaShooterTypes/CombatState.h"

#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f;

class AWeapon;
class AArenaShooterCharacter;
class AArenaShooterPlayerController;
class AArenaShooterHUD;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENASHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	AArenaShooterCharacter* OwningCharacter = nullptr;
	UPROPERTY()
	AArenaShooterPlayerController* OwningController = nullptr;
	UPROPERTY()
	AArenaShooterHUD* OwningHUD = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon = nullptr;
	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float AimWalkSpeed;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bFireButtonPressed;

	FVector HitTarget;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	/*
	 * HUD and Crosshairs
	 */
	
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	FHUDPackage HUDPackage;

	/*
	 * Aiming and FOV
	 */

	// FOV when not aiming; set to the camera's base FOV in BeginPlay
	float DefaultFOV;
	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed = 20.f;

	/*
	 * Automatic fire
	 */

	FTimerHandle FireTimer;
	
	bool bCanFire = true;
	// CarriedAmmo for currently equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;
	
	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	TMap<EWeaponType, int32> CarriedAmmoMap;

public:
	UCombatComponent();

	friend class AArenaShooterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void EquipWeapon(AWeapon* WeaponToEquip);
	void Reload();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION()
	void OnRep_CombatState();

	

protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	void Fire();

	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
	void ServerReload();
	
	void HandleReload();

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	int32 GetAmountToReload();

private:
	UFUNCTION()
	void OnRep_CarriedAmmo();
	
	void InterpFOV(float DeltaTime);

	void StartFireTimer();
	void FireTimerFinished();

	bool CanFire();

	void InitializeCarriedAmmo();

	void UpdateAmmoValues();
		
};
