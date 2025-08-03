// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Game/SurvivalUI.h"

#include "Components/TextBlock.h"
#include "Weapons/WeaponComponent.h"

void USurvivalUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	auto* LocalWeaponComponent = GetWorld()->GetFirstPlayerController()->GetPawn()->FindComponentByClass<UWeaponComponent>();
	ensure(LocalWeaponComponent);

	LocalWeaponComponent->OnAmmoChanged.AddUObject(this, &ThisClass::OnAmmoChanged);
}

void USurvivalUI::OnAmmoChanged(uint16 NewAmmoCount)
{
	AmmoDisplay->SetText(FText::FromString(FString::FromInt(NewAmmoCount)));
}
