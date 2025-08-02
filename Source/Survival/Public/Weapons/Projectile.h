// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

/**
 * Main parent class of Weapon Projectile Actors
 */
UCLASS()
class SURVIVAL_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();

	// Call when spawning a Projectile
	void SetProperties(float ProjectileDamage, float MuzzleExitSpeed, float LifeSpan);
	
protected:
	UFUNCTION()
	void OnProjectileImpact(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Unreliable)
	void MC_ImpactEffects();
	
	void ApplyDamage(AActor* OtherActor);
	
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	float Damage = 10.0f;
};
