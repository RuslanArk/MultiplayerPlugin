// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"

#include "ArenaShooter/ArenaShooterTypes/ArenaShooterEnums.h"
#include "ArenaShooter/Interfaces/InteractWithCrosshairInterface.h"

#include "ArenaShooterCharacter.generated.h"

class UAnimMontage;
class UCameraComponent;
class USpringArmComponent;
class USoundCue;
class UWidgetComponent;

class AArenaShooterPlayerController;
class AArenaShooterPlayerState;
class UCombatComponent;
class AWeapon;

UCLASS()
class ARENASHOOTER_API AArenaShooterCharacter : public ACharacter, public IInteractWithCrosshairInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoom = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* FollowCamera = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadWidget = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon = nullptr;

	UPROPERTY(VisibleAnywhere)
	UCombatComponent* Combat = nullptr;

	/**
	 * Animation montages
	 */
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* FireWeaponMontage = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReactMontage = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ElimMontage = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ReloadMontage = nullptr;

	float InterpAO_Yaw;
	float AO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;

	/**
	 * Player Health
	 */

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "PlayerStats")
	float Health = 100.f;

	bool bElimed = false;
	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	UPROPERTY(VisibleAnywhere)
	AArenaShooterPlayerController* ASPlayerController = nullptr;

	/**
	 * Dissolve effect
	 */
	
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline = nullptr;
	
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve = nullptr;
	// Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = "Elim")
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance = nullptr;
	// Material Instance set on the Blueprint, used with the dynamic material instance
	UPROPERTY(EditAnywhere, Category = "Elim")
	UMaterialInstance* DissolveMaterialInstance = nullptr;
	
	/**
    * Elim bot
    */
    UPROPERTY(EditAnywhere)
    UParticleSystem* ElimBotEffect = nullptr;
    UPROPERTY(VisibleAnywhere)
    UParticleSystemComponent* ElimBotComponent = nullptr;
	UPROPERTY(EditAnywhere)
	USoundCue* ElimBotSound = nullptr;

	UPROPERTY()
	AArenaShooterPlayerState* CharacterPlayerState = nullptr;
	
public:
	AArenaShooterCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;
	
	void SetOverlappingWeapon(AWeapon* Weapon);

	bool IsWeaponEquipped();
	bool IsAiming();

	AWeapon* GetEquippedWeapon();
	void PlayFireMontage(bool bAiming);
	void PlayElimMontage();
	void PlayReloadMontage();

	FORCEINLINE float GetAOYaw() const { return AO_Yaw; }
	FORCEINLINE float GetAOPitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimed() const { return bElimed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	FVector GetHitTarget() const;

	virtual void OnRep_ReplicatedMovement() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	void Elim();
	void ElimTimerFinished();

protected:
	virtual void BeginPlay() override;

	virtual void Jump() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void CalculateAO_Pitch();

	void AimOffset(float DeltaTime);
	void SimProxiesTurn();
	
	void PlayHitReactMontage();

	void UpdateHUDHealth();
	// Poll for any relevant classes and initialize our HUD
	void PollInit();
	
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

private:
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
	UFUNCTION()
	void OnRep_Health();

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	void TurnInPlace(float DeltaTime);

	void HideCameraIfCharacterClose();

	float CalculateSpeed();

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();
	
};

