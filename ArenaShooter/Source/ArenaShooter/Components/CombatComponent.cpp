// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "ArenaShooter/Character/ArenaShooterCharacter.h"
#include "ArenaShooter/Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogCombatComp, All, All);

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwningCharacter)
	{
		OwningCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
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

