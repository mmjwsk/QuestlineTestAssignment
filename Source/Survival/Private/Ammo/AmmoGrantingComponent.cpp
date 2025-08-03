// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo/AmmoGrantingComponent.h"

#include "Weapons/WeaponComponent.h"


void UAmmoGrantingComponent::GrantAmmo(FGameplayTag WeaponToGrantAmmoFor, UWeaponComponent* WeaponComponentToGrantTo) const
{
	WeaponComponentToGrantTo->AddAmmo(WeaponToGrantAmmoFor, AmmoAmount);
}
