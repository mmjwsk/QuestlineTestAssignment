// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WeaponData.h"
#include "WeaponInventoryData.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class UWeaponDatabase;

DECLARE_LOG_CATEGORY_EXTERN(LogWeaponComponent, Log, All);
DECLARE_MULTICAST_DELEGATE_OneParam(FAmmoChangedDelegate, uint16 /*CurrentAmmoCount*/);

/**
 * Main component responsible for Character's weapon behavior
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SURVIVAL_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

	FAmmoChangedDelegate OnAmmoChanged;

	void StartFire();
	void StopFire();
	void AddNewWeapon(FGameplayTag NewWeaponTag);
	void AddAmmo(FGameplayTag WeaponTag, uint16 AmountToAdd);
	void ScrollWeapon();

	UFUNCTION(Server, Reliable)
	void S_Fire();

	UFUNCTION(Server, Reliable)
	void S_ScrollWeapon();

	UFUNCTION(NetMulticast, Unreliable)
	void MC_FireEffects();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void Fire();
	void HandleFiring();
	void AddWeaponProp(FGameplayTag WeaponTag);
	void AttachPropToPlayer(AWeaponProp* Prop);
	void ConsumeCurrentWeaponAmmo(uint16 AmountToConsume);

	uint16 GetCurrentWeaponAmmo() const;
	bool HasWeaponEquipped() const { return CurrentWeaponIndex != 255; }

	UFUNCTION()
	void OnRep_WeaponInventory() const;

	UFUNCTION()
	void OnRep_EquippedWeaponProp();
	
	UPROPERTY(EditAnywhere, Category = "Weapons")
	FName FirstPersonWeaponSocket = FName("HandGrip_R");

	UPROPERTY(EditAnywhere, Category = "Weapons")
	FName ThirdPersonWeaponSocket = FName("HandGrip_R");

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TObjectPtr<UWeaponDatabase> Database;

	UPROPERTY(Replicated)
	bool bIsFiring = false;
	
	UPROPERTY(Replicated)
	uint8 CurrentWeaponIndex = 255;
	
	UPROPERTY(Replicated)
	TArray<AWeaponProp*> WeaponProps;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeaponProp)
	AWeaponProp* EquippedWeaponProp;

	// Replicated so that the Server verifies the ammo count to prevent client-side infinite ammo cheats or spawning in weapons
	//UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_WeaponInventory)
	//TArray<FWeaponInventoryData> WeaponInventory;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponInventory)
	FReplicatedWeaponInventory WeaponInventory;

	FWeaponData CurrentWeaponData;
	FTimerHandle FireRateTimerHandle;

private:
	// Delays attachment on Client to handle cases where Mesh is not yet replicated so attachment fails
	void SafeAttachToSocket(USceneComponent* Child, USceneComponent* Parent, const FName& SocketName,
		const FAttachmentTransformRules& AttachmentRules, float DelaySeconds);
};
