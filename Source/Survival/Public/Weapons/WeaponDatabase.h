// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WeaponData.h"
#include "Engine/DataAsset.h"
#include "WeaponDatabase.generated.h"

/**
 * Data Asset containing all weapon-relevant data.
 */
UCLASS()
class SURVIVAL_API UWeaponDatabase : public UDataAsset
{
	GENERATED_BODY()

public:
	// Tries to get Data of a Weapon based on provided Gameplay Tag. Returns false if Weapon does not exist in the DB.
	bool TryGetWeaponData(const FGameplayTag& WeaponTag, FWeaponData& OutWeaponData);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Data")
	TMap<FGameplayTag, FWeaponData> Weapons;
};
