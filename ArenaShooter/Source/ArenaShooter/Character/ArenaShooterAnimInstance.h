// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ArenaShooterAnimInstance.generated.h"

class AArenaShooterCharacter;

UCLASS()
class ARENASHOOTER_API UArenaShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	AArenaShooterCharacter* ArenaShooterCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;
	
public:
	// the same as BeginPlay
	virtual void NativeInitializeAnimation() override;
	// the same as Tick
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
