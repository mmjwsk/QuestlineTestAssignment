// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Game/SurvivalUI.h"

#include "Attributes/HealthComponent.h"
#include "Components/TextBlock.h"
#include "Weapons/WeaponComponent.h"

void USurvivalUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	auto* LocalWeaponComponent = GetWorld()->GetFirstPlayerController()->GetPawn()->FindComponentByClass<UWeaponComponent>();
	ensure(LocalWeaponComponent);
	LocalWeaponComponent->OnAmmoChanged.AddUObject(this, &ThisClass::OnAmmoChanged);

	auto* LocalHealthComponent = GetWorld()->GetFirstPlayerController()->GetPawn()->FindComponentByClass<UHealthComponent>();
	ensure(LocalHealthComponent);
	LocalHealthComponent->OnHealthChanged.AddUObject(this, &ThisClass::OnHealthChanged);
}

void USurvivalUI::OnAmmoChanged(uint16 NewAmmoCount)
{
	AmmoDisplay->SetText(FText::FromString(FString::FromInt(NewAmmoCount)));
}

void USurvivalUI::OnHealthChanged(float NewHealth)
{
	HealthDisplay->SetText(FText::FromString(FString::SanitizeFloat(NewHealth, 0)));
}
