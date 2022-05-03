// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


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
	
	PlayerInputComponent->BindAxis("MoveForward", this, &AArenaShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AArenaShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AArenaShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AArenaShooterCharacter::LookUp);
	
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

