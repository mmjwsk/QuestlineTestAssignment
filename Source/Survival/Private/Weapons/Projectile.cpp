// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName("Projectile");
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnProjectileOverlap);
	CollisionComponent->SetupAttachment(RootComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
}

void AProjectile::OnProjectileOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (!HasAuthority()) return;

	if (OtherActor && OtherActor != GetOwner())
	{
		ApplyDamage(OtherActor);
	}

	MC_ImpactEffects();

	Destroy();
}

void AProjectile::SetProperties(float ProjectileDamage, float MuzzleExitSpeed, float LifeSpan)
{
	ProjectileMovement->MaxSpeed = MuzzleExitSpeed;
	ProjectileMovement->InitialSpeed = MuzzleExitSpeed;
	Damage = ProjectileDamage;
	InitialLifeSpan = LifeSpan;

	if (GetInstigator())
	{
		CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
	}

	SetReplicateMovement(true);
}

void AProjectile::ApplyDamage(AActor* OtherActor)
{
	UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
}

void AProjectile::MC_ImpactEffects_Implementation()
{
	// Here would be some nice VFX & SFX of projectile impacts
}
