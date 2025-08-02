// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WeaponData.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class UWeaponDatabase;

DECLARE_LOG_CATEGORY_EXTERN(LogWeaponComponent, Log, All);

USTRUCT()
struct FWeaponInventoryData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FGameplayTag WeaponTag = FGameplayTag::EmptyTag;

	// Exists because players can collect ammo for weapons they don't yet have
	UPROPERTY(VisibleAnywhere)
	bool bWeaponCollected = false;

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
	void AddNewWeapon(FGameplayTag NewWeaponTag);
	void ScrollWeapon();

	UFUNCTION(Server, Reliable)
	void S_Fire();

	UFUNCTION(Server, Reliable)
	void S_ScrollWeapon();

	UFUNCTION(NetMulticast, Unreliable)
	void MC_FireEffects();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void Fire();
	void HandleFiring();
	void AddWeaponProp(FGameplayTag WeaponTag);
	void AttachPropToPlayer(AWeaponProp* Prop);

	uint16& GetAmmo() { return WeaponInventory[CurrentWeaponIndex].AmmoCount; }
	bool HasWeaponEquipped() const { return CurrentWeaponIndex != 255; }

	UFUNCTION()
	void OnRep_WeaponInventory();

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
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_WeaponInventory)
	TArray<FWeaponInventoryData> WeaponInventory;

	FWeaponData CurrentWeaponData;
	FTimerHandle FireRateTimerHandle;

private:
	// Delays attachment on Client to handle cases where Mesh is not yet replicated so attachment fails
	void SafeAttachToSocket(USceneComponent* Child, USceneComponent* Parent, const FName& SocketName,
		const FAttachmentTransformRules& AttachmentRules, float DelaySeconds);
};
