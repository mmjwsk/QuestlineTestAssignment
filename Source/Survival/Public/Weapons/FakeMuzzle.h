// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FakeMuzzle.generated.h"

/**
 * Exists solely to have a strong-typed muzzle fakeout due to Anim BPs being broken in this test assignment
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SURVIVAL_API UFakeMuzzle : public USceneComponent
{
	GENERATED_BODY()
};
