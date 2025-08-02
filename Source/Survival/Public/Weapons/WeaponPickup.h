// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Pickups/PickupBase.h"
#include "WeaponPickup.generated.h"

UCLASS()
class SURVIVAL_API AWeaponPickup : public APickupBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	FGameplayTag WeaponTag;

	virtual bool TryHandlePickup(ASurvivalCharacter* PickupCharacter) override;
};
