// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Pickups/PickupBase.h"
#include "WeaponPickup.generated.h"

class UAmmoGrantingComponent;

UCLASS()
class SURVIVAL_API AWeaponPickup : public APickupBase
{
	GENERATED_BODY()

public:
	AWeaponPickup();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	FGameplayTag WeaponTag;

	// Added here cause we want to receive some ammo when we pick up a weapon as well
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<UAmmoGrantingComponent> AmmoGrantingComponent;

	virtual bool TryHandlePickup(ASurvivalCharacter* PickupCharacter) override;
};
