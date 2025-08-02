// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponPickup.h"

#include "Core/Player/SurvivalCharacter.h"
#include "Weapons/WeaponComponent.h"


bool AWeaponPickup::TryHandlePickup(ASurvivalCharacter* PickupCharacter)
{
	auto* WeaponComponent = PickupCharacter->FindComponentByClass<UWeaponComponent>();
	ensure(WeaponComponent);

	WeaponComponent->AddNewWeapon(WeaponTag);
	return true;
}
