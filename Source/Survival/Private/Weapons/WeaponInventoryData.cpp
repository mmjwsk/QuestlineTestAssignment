// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponInventoryData.h"

template<>
struct TStructOpsTypeTraits<FReplicatedWeaponInventory> : public TStructOpsTypeTraitsBase2<FReplicatedWeaponInventory>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};