// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

class UCapsuleComponent;
class ASurvivalCharacter;
/**
 * Base class for Pickup Actors
 * NOTE: I chose for pickups to work on proximity collider as this is way more in line with fast-paced arena shooter gameplay.
 * However if the preferred design was to have them work on keypress interaction, I'd go for an IInteractable interface.
 */
UCLASS(Abstract)
class SURVIVAL_API APickupBase : public AActor
{
	GENERATED_BODY()

public:
	APickupBase();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	float RespawnTime = 15.0f;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	virtual bool TryHandlePickup(ASurvivalCharacter* PickupCharacter) PURE_VIRTUAL(APickupBase::TryHandlePickup, return false;);

	virtual void ActivatePickup();
	virtual void DectivatePickup();

	UFUNCTION()
	void OnRep_ActiveState();

	UFUNCTION(NetMulticast, Reliable)
	void MC_SetPickupActive(bool bNewActive);

	UPROPERTY(ReplicatedUsing = OnRep_ActiveState)
	bool bIsActive = true;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> TriggerCollider;

	FTimerHandle RespawnTimerHandle;
};
