// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WeaponData.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

USTRUCT()
struct FWeaponInventoryData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FGameplayTag WeaponTag = FGameplayTag::EmptyTag;

	UPROPERTY(VisibleAnywhere)
	uint16 AmmoCount = 0;
};

/**
 * Main component responsible for Character's weapon behavior
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SURVIVAL_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();
	
	void StartFire();
	void StopFire();

	UFUNCTION(Server, Reliable)
	void S_Fire();

	UFUNCTION(NetMulticast, Unreliable)
	void MC_FireEffects();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void Fire();
	void HandleFiring();

	uint16& GetAmmo() { return WeaponInventory[CurrentWeaponIndex].AmmoCount; }
	bool HasWeaponEquipped() const { return CurrentWeaponIndex != 255; }

	FTimerHandle FireRateTimerHandle;

	UPROPERTY(Replicated)
	bool bIsFiring = false;

	UFUNCTION()
	void OnRep_WeaponInventory();

protected:

	uint8 CurrentWeaponIndex = 255;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_WeaponInventory)
	TArray<FWeaponInventoryData> WeaponInventory;

	const FWeaponData* CurrentWeaponData;

private:
	bool HasAuthority() const;
};
