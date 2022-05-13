// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "ArenaShooter/Character/ArenaShooterCharacter.h"
#include "ArenaShooter/Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (!OwningCharacter || !WeaponToEquip) return;
	EquipedWeapon = WeaponToEquip;

	if (const USkeletalMeshSocket* HandSocket = OwningCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket")))
	{
		HandSocket->AttachActor(EquipedWeapon, OwningCharacter->GetMesh());
		EquipedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		EquipedWeapon->SetOwner(OwningCharacter);
	}	
}

