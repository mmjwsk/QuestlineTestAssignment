// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "UObject/Object.h"
#include "WeaponInventoryData.generated.h"

class UWeaponComponent;
// Optimized for replication via FastArraySerializer
USTRUCT()
struct FWeaponInventoryItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FGameplayTag WeaponTag = FGameplayTag::EmptyTag;

	// Exists because players can collect ammo for weapons they don't yet have
	UPROPERTY(VisibleAnywhere)
	bool bWeaponCollected = false;

	UPROPERTY(VisibleAnywhere)
	uint16 AmmoCount = 0;

	bool operator==(const FWeaponInventoryItem& Other) const
	{
		return WeaponTag == Other.WeaponTag;
	}
};

USTRUCT()
struct FReplicatedWeaponInventory : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<FWeaponInventoryItem> Items;

	UPROPERTY(VisibleAnywhere)
	UWeaponComponent* OwnerComponent = nullptr;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FWeaponInventoryItem, FReplicatedWeaponInventory>(Items, DeltaParms, *this);
	}
};