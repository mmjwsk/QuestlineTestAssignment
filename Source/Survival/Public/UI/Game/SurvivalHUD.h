// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SurvivalUI.h"
#include "GameFramework/HUD.h"
#include "SurvivalHUD.generated.h"


/**
 * HUD displayed during match
 */
UCLASS(Abstract)
class SURVIVAL_API ASurvivalHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USurvivalUI> SurvivalUI;

	virtual void BeginPlay() override;
};
