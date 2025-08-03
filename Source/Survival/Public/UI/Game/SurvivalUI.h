// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalUI.generated.h"

class UTextBlock;

/**
 * Minimal UI displayed during game session
 */
UCLASS()
class SURVIVAL_API USurvivalUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	
	void OnAmmoChanged(uint16 NewAmmoCount);
	void OnHealthChanged(float NewHealth);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> AmmoDisplay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> HealthDisplay;
};
