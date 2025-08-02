// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AmmoGrantingComponent.generated.h"

/**
 * Lightweight component that handles giving Ammo - reusable on Weapons, Pickups etc.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SURVIVAL_API UAmmoGrantingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void GrantAmmo();
};
