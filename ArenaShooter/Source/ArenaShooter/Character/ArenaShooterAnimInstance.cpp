// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaShooterAnimInstance.h"

#include "ArenaShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	bIsCrouched = ArenaShooterCharacter->bIsCrouched;
	bIsAiming = ArenaShooterCharacter->IsAiming();

	// Offset Yaw for strafing
	FRotator AimRotation = ArenaShooterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ArenaShooterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ArenaShooterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = ArenaShooterCharacter->GetAOYaw();
	AO_Pitch = ArenaShooterCharacter->GetAOPitch();
	
}
