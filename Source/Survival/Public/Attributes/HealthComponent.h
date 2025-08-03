// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAttributes, Log, All);

DECLARE_MULTICAST_DELEGATE(FOnPlayerDeath);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float /*HealthValue*/)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SURVIVAL_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	FOnPlayerDeath OnDeath;
	FOnHealthChanged OnHealthChanged;
	
	void Respawn();
	float GetHealth() const { return Health; }

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void ApplyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	
	UFUNCTION()
	void OnRep_Health() const;

	UFUNCTION()
	void HandleDeath();

	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float Health;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(Replicated)
	bool bIsDead = false;
};
