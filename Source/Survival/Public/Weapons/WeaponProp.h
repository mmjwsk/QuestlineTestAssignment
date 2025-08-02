// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponProp.generated.h"

/**
 * Actor representing a visualization of a Weapon. Cosmetic-only - does not contain logic.
 */
UCLASS()
class SURVIVAL_API AWeaponProp : public AActor
{
	GENERATED_BODY()

public:
	AWeaponProp();

	USkeletalMeshComponent* GetFPPMesh() const { return FirstPersonMesh; }
	USkeletalMeshComponent* GetTPPMesh() const { return ThirdPersonMesh; }

protected:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ThirdPersonMesh;
};
