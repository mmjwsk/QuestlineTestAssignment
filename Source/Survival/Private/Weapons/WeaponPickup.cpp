// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponPickup.h"

#include "Ammo/AmmoGrantingComponent.h"
#include "Core/Player/SurvivalCharacter.h"
#include "Weapons/WeaponComponent.h"


AWeaponPickup::AWeaponPickup()
{
	AmmoGrantingComponent = CreateDefaultSubobject<UAmmoGrantingComponent>(TEXT("AmmoGrantingComponent"));
}

// Technically with the current state of implementation WeaponPickup could inherit AmmoPickup and reuse much of the (rather sparse) code there.
// However I don't think that this is a good design in the long run, as those pickups are too different conceptually
// Hence I kept them as parallel children instead of inheriting one another, despite noticing the slight reuse of code.
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
