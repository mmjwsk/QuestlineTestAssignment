// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo/AmmoPickup.h"
#include "Ammo/AmmoGrantingComponent.h"
#include "Core/Player/SurvivalCharacter.h"
#include "Weapons/WeaponComponent.h"


AAmmoPickup::AAmmoPickup()
{
	AmmoGrantingComponent = CreateDefaultSubobject<UAmmoGrantingComponent>(TEXT("AmmoGrantingComponent"));
}

bool AAmmoPickup::TryHandlePickup(ASurvivalCharacter* PickupCharacter)
{
	if (!HasAuthority()) return false;

	auto* WeaponComponent = PickupCharacter->FindComponentByClass<UWeaponComponent>();
	ensure(WeaponComponent);

	AmmoGrantingComponent->GrantAmmo(WeaponTag, WeaponComponent);

	UE_LOG(LogTemp, Warning, TEXT("Ammo Pickup Interacted: %s"), *GetName());
	return true;
}



