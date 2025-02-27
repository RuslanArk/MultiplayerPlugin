// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaShooterAnimInstance.h"

#include "ArenaShooterCharacter.h"
#include "ArenaShooter/Weapon/Weapon.h"
#include "ArenaShooter/ArenaShooterTypes/CombatState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UArenaShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ArenaShooterCharacter = Cast<AArenaShooterCharacter>(TryGetPawnOwner());

	bLocallyControlled = false;
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
	EquippedWeapon = ArenaShooterCharacter->GetEquippedWeapon();
	bIsCrouched = ArenaShooterCharacter->bIsCrouched;
	bIsAiming = ArenaShooterCharacter->IsAiming();
	TurningInPlace = ArenaShooterCharacter->GetTurningInPlace();
	bRotateRootBone = ArenaShooterCharacter->ShouldRotateRootBone();
	bElimed = ArenaShooterCharacter->IsElimed();

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

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && ArenaShooterCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		ArenaShooterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (ArenaShooterCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - ArenaShooterCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaSeconds, 30.f);
		}
	}

	bUseFabrik = ArenaShooterCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bUseAimOffsets = ArenaShooterCharacter->GetCombatState() != ECombatState::ECS_Reloading && !ArenaShooterCharacter->IsGameplayDisabled();
	bTransformRightHand = ArenaShooterCharacter->GetCombatState() != ECombatState::ECS_Reloading && !ArenaShooterCharacter->IsGameplayDisabled();
		
}
