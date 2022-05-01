// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ArenaShooterCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ARENASHOOTER_API AArenaShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AArenaShooterCharacter();
	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	virtual void BeginPlay() override;
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* FollowCamera;
	
	
};
