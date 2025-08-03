// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/HealthComponent.h"

#include "Core/Game/GameModeGame.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogAttributes)

UHealthComponent::UHealthComponent()
{
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	OnRep_Health();

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::ApplyDamage);
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, Health);
	DOREPLIFETIME(UHealthComponent, bIsDead);
}

void UHealthComponent::ApplyDamage(AActor*, float Damage, const UDamageType*, AController*, AActor*)
{
	if (!GetOwner()->HasAuthority() || bIsDead) return;

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	OnRep_Health();

	UE_LOG(LogAttributes, Warning, TEXT("Damage %f taken by %s"), Damage, *GetOwner()->GetName());

	if (Health <= 0.0f && !bIsDead)
	{
		bIsDead = true;
		HandleDeath();
	}
}

void UHealthComponent::HandleDeath()
{
	if (!IsValid(GetOwner())) return;
	
	OnDeath.Broadcast();
	
	UE_LOG(LogAttributes, Warning, TEXT("%s KILLED"), *GetOwner()->GetName());

	// Disable actor behavior
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		Character->DisableInput(nullptr);
		Character->SetLifeSpan(5.f);
	}

	// Notify GameMode
	if (GetOwner()->HasAuthority())
	{
		if (AController* CharacterController = Character->GetController())
		{
			if (auto* GameMode = GetWorld()->GetAuthGameMode<AGameModeGame>())
			{
				GameMode->HandlePlayerDeath(CharacterController);
			}
		}
	}
}

void UHealthComponent::OnRep_Health() const
{
	OnHealthChanged.Broadcast(GetHealth());
}

void UHealthComponent::OnRep_IsDead()
{
	if (bIsDead)
	{
		HandleDeath();
	}
}
