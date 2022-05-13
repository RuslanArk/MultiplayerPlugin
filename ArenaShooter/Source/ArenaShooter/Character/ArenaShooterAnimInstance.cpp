// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaShooterAnimInstance.h"

#include "ArenaShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UArenaShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ArenaShooterCharacter = Cast<AArenaShooterCharacter>(TryGetPawnOwner());
}

void UArenaShooterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ArenaShooterCharacter == nullptr)
	{
		ArenaShooterCharacter = Cast<AArenaShooterCharacter>(TryGetPawnOwner());
		if (ArenaShooterCharacter == nullptr) return;
	}

	FVector Velocity = ArenaShooterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = ArenaShooterCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = ArenaShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

	bWeaponEquipped = ArenaShooterCharacter->IsWeaponEquipped();
}
