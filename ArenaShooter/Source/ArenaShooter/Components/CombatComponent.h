// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;
class AArenaShooterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENASHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	AArenaShooterCharacter* OwningCharacter = nullptr;
	AWeapon* EquipedWeapon = nullptr;

public:
	UCombatComponent();

	friend class AArenaShooterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EquipWeapon(AWeapon* WeaponToEquip);

protected:
	virtual void BeginPlay() override;
		
};
