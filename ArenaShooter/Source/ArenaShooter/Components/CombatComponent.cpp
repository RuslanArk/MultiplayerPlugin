// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "ArenaShooter/Character/ArenaShooterCharacter.h"
#include "ArenaShooter/PlayerController/ArenaShooterPlayerController.h"
#include "ArenaShooter/Weapon/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogCombatComp, All, All);

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;

	CrosshairVelocityFactor = 0.0f;
	CrosshairInAirFactor = 0.0f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwningCharacter)
	{
		OwningCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		if (OwningCharacter->GetFollowCamera())
		{
			DefaultFOV = OwningCharacter->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwningCharacter && OwningCharacter->IsLocallyControlled())
	{		
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}	
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (!OwningCharacter || !WeaponToEquip) return;
	EquippedWeapon = WeaponToEquip;

	if (const USkeletalMeshSocket* HandSocket = OwningCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket")))
	{
		HandSocket->AttachActor(EquippedWeapon, OwningCharacter->GetMesh());
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		EquippedWeapon->SetOwner(OwningCharacter);
		OwningCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		OwningCharacter->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (OwningCharacter)
	{
		OwningCharacter->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (OwningCharacter)
	{
		OwningCharacter->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && OwningCharacter)
	{
		OwningCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		OwningCharacter->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);		
		ServerFire(HitResult.ImpactPoint);

		if (EquippedWeapon)
		{
			CrosshairShootingFactor = 0.75f;
		}
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (!EquippedWeapon) return;	
	if (OwningCharacter)
	{
		OwningCharacter->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenDeprojected = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (!bScreenDeprojected) return;

	FVector Start = CrosshairWorldPosition;
	if (OwningCharacter)
	{
		float DistanceToCharacter = (OwningCharacter->GetActorLocation() - Start).Size();
		Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
	}
	FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
	GetWorld()->LineTraceSingleByChannel(
		TraceHitResult,
		Start,
		End,
		ECC_Visibility);

	if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairInterface>())
	{
		HUDPackage.CrossHairsColor = FLinearColor::Red;
	}
	else
	{
		HUDPackage.CrossHairsColor = FLinearColor::White;
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (OwningCharacter == nullptr && OwningCharacter->Controller == nullptr) return;

	OwningController = OwningController == nullptr ? Cast<AArenaShooterPlayerController>(OwningCharacter->Controller) : OwningController;
	if (OwningController)
	{
		OwningHUD = OwningHUD == nullptr ? Cast<AArenaShooterHUD>(OwningController->GetHUD()) : OwningHUD;
		if (OwningHUD)
		{			
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairBottom = EquippedWeapon->CrosshairsBottom;
				HUDPackage.CrosshairCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairTop = EquippedWeapon->CrosshairsTop;
			}
			else
			{
				HUDPackage.CrosshairBottom = nullptr;
				HUDPackage.CrosshairCenter = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairRight = nullptr;
				HUDPackage.CrosshairTop = nullptr;				
			}
			// Calculate Crosshair spread

			// [0, 600] -> [0, 1]
			FVector2D WalkSpeedRange(0.f, OwningCharacter->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange { 0.f, 1.f };
			FVector Velocity = OwningCharacter->GetVelocity();
			Velocity.Z = 0.0f;
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if (OwningCharacter->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.0f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.0f, DeltaTime, 30.f);
			}

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.0f, DeltaTime, 20.f);
			
			HUDPackage.CrosshairSpread =
					0.5f +
					CrosshairVelocityFactor +
					CrosshairInAirFactor -
					CrosshairAimFactor +
					CrosshairShootingFactor;
			
			OwningHUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (!EquippedWeapon) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	if (OwningCharacter && OwningCharacter->GetFollowCamera())
	{
		OwningCharacter->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}
