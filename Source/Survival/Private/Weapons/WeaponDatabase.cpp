// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponDatabase.h"

bool UWeaponDatabase::TryGetWeaponData(const FGameplayTag& WeaponTag, FWeaponData& OutWeaponData)
{
	if (Weapons.Contains(WeaponTag))
	{
		OutWeaponData = Weapons[WeaponTag];
		return true;
	}
	else return false;
}
