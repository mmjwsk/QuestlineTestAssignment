// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Game/SurvivalHUD.h"
#include "Blueprint/UserWidget.h"

void ASurvivalHUD::BeginPlay()
{
	Super::BeginPlay();

	if (auto* Widget = CreateWidget<UUserWidget>(GetWorld(), SurvivalUI))
	{
		Widget->AddToViewport(9999);
	}
}
