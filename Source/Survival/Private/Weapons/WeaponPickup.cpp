// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponPickup.h"

#include "Ammo/AmmoGrantingComponent.h"
#include "Core/Player/SurvivalCharacter.h"
#include "Weapons/WeaponComponent.h"


AWeaponPickup::AWeaponPickup()
{
	AmmoGrantingComponent = CreateDefaultSubobject<UAmmoGrantingComponent>(TEXT("AmmoGrantingComponent"));
}

bool AWeaponPickup::TryHandlePickup(ASurvivalCharacter* PickupCharacter)
{
	if (!HasAuthority()) return false;
	
	auto* WeaponComponent = PickupCharacter->FindComponentByClass<UWeaponComponent>();
	ensure(WeaponComponent);

	AmmoGrantingComponent->GrantAmmo(WeaponTag, WeaponComponent);
	WeaponComponent->AddNewWeapon(WeaponTag);

	UE_LOG(LogTemp, Warning, TEXT("Weapon Pickup Interacted: %s"), *GetName());
	return true;
}
