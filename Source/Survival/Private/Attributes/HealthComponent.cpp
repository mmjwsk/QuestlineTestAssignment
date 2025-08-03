// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/HealthComponent.h"

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
	OnDeath.Broadcast();
	UE_LOG(LogAttributes, Warning, TEXT("%s KILLED"), *GetOwner()->GetName());

	// Example: disable movement, ragdoll, etc.
	AActor* Owner = GetOwner();
	if (Owner)
	{
		Owner->SetLifeSpan(5.0f); // optional, or call respawn from GameMode
	}
}

void UHealthComponent::Respawn()
{
	if (!GetOwner()->HasAuthority()) return;

	Health = MaxHealth;
	bIsDead = false;
	OnRep_Health();
}

void UHealthComponent::OnRep_Health() const
{
	OnHealthChanged.Broadcast(GetHealth());
}
