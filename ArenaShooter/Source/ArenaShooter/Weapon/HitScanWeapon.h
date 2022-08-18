// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

class UParticleSystem;
class USoundCue;

UCLASS()
class ARENASHOOTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;

public:
	virtual void Fire(const FVector& HitTarget) override;
	
};
