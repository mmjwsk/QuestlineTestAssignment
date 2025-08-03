// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Pickups/PickupBase.h"
#include "AmmoPickup.generated.h"

class UAmmoGrantingComponent;

UCLASS()
class SURVIVAL_API AAmmoPickup : public APickupBase
{
	GENERATED_BODY()

public:
	AAmmoPickup();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	FGameplayTag WeaponTag;
	
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<UAmmoGrantingComponent> AmmoGrantingComponent;

	virtual bool TryHandlePickup(ASurvivalCharacter* PickupCharacter) override;
};
