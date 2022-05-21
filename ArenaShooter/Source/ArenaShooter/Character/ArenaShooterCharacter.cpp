// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaShooterCharacter.h"

#include "ArenaShooter/Components/CombatComponent.h"
#include "ArenaShooter/Weapon/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"


AArenaShooterCharacter::AArenaShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Character Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Overhead Widget"));
	OverheadWidget->SetupAttachment(GetRootComponent());

	Combat = CreateDefaultSubobject<UCombatComponent>("Combat Component");
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AArenaShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AArenaShooterCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void AArenaShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArenaShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArenaShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (!PlayerInputComponent) return;

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AArenaShooterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AArenaShooterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AArenaShooterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AArenaShooterCharacter::AimButtonReleased);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &AArenaShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AArenaShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AArenaShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AArenaShooterCharacter::LookUp);
	
}

void AArenaShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->OwningCharacter = this;
	}
}

void AArenaShooterCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.0, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);		
	}
}

void AArenaShooterCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.0, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void AArenaShooterCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AArenaShooterCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AArenaShooterCharacter::EquipButtonPressed()
{
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void AArenaShooterCharacter::CrouchButtonPressed()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}	
}

void AArenaShooterCharacter::AimButtonPressed()
{
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void AArenaShooterCharacter::AimButtonReleased()
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void AArenaShooterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled() && OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
}

void AArenaShooterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}	

	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void AArenaShooterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

bool AArenaShooterCharacter::IsWeaponEquipped()
{
	return Combat && Combat->EquippedWeapon;
}

bool AArenaShooterCharacter::IsAiming()
{
	return Combat && Combat->bAiming;
}


