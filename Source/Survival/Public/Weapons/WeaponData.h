// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponData.generated.h"

class AWeaponProp;
class AProjectile;

/**
 * Data containing properties of a specific Weapon type
 */
USTRUCT()
struct SURVIVAL_API FWeaponData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AWeaponProp> WeaponProp;

	UPROPERTY(EditAnywhere)
	float Damage = 10.f;

	UPROPERTY(EditAnywhere)
	uint16 MaxAmmo = 100;

	// Whether the weapon operates on Hitscan or Projectile mechanics
	UPROPERTY(EditAnywhere)
	bool bHitscan = false;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bHitscan", EditConditionHides))
	TSoftClassPtr<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bHitscan", EditConditionHides))
	float ProjectileMuzzleSpeed = 1000.f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bHitscan", EditConditionHides))
	float ProjectileLifeSpan = 5.f;

	// Whether the weapon can fire continuously when Fire action is held
	UPROPERTY(EditAnywhere)
	bool bIsAutomatic = false;

	// Delay between shots [s]. Relevant only for automatic weapons.
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsAutomatic", EditConditionHides))
	float RateOfFire = 0.1f;
};
