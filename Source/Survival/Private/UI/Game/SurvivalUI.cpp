// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Game/SurvivalUI.h"

#include "Attributes/HealthComponent.h"
#include "Components/TextBlock.h"
#include "Core/Player/SurvivalPlayerController.h"
#include "Weapons/WeaponComponent.h"

class AGameModeGame;

void USurvivalUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto* PlayerController = Cast<ASurvivalPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PlayerController->OnPossessed.AddUObject(this, &ThisClass::OnPlayerPawnPossessed);
		SubscribeLocalPlayerEvents(PlayerController->GetPawn());
	}
}

void USurvivalUI::OnAmmoChanged(uint16 NewAmmoCount)
{
	AmmoDisplay->SetText(FText::FromString(FString::FromInt(NewAmmoCount)));
}

void USurvivalUI::OnHealthChanged(float NewHealth)
{
	HealthDisplay->SetText(FText::FromString(FString::SanitizeFloat(NewHealth, 0)));
}

void USurvivalUI::OnPlayerPawnPossessed(APawn* Pawn)
{
	SubscribeLocalPlayerEvents(Pawn);
}

void USurvivalUI::OnPlayerDied()
{
	WastedDisplay->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USurvivalUI::SubscribeLocalPlayerEvents(const APawn* LocalPawn)
{
	if (!IsValid(LocalPawn)) return;
	
	WastedDisplay->SetVisibility(ESlateVisibility::Collapsed);
	
	auto* LocalWeaponComponent = LocalPawn->FindComponentByClass<UWeaponComponent>();
	ensure(LocalWeaponComponent);
	LocalWeaponComponent->OnAmmoChanged.AddUObject(this, &ThisClass::OnAmmoChanged);

	OnAmmoChanged(LocalWeaponComponent->GetCurrentWeaponAmmo());

	auto* LocalHealthComponent = LocalPawn->FindComponentByClass<UHealthComponent>();
	ensure(LocalHealthComponent);
	LocalHealthComponent->OnHealthChanged.AddUObject(this, &ThisClass::OnHealthChanged);
	LocalHealthComponent->OnDeath.AddUObject(this, &ThisClass::OnPlayerDied);

	OnHealthChanged(LocalHealthComponent->GetHealth());
}
