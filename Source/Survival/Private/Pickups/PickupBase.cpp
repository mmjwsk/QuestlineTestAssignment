// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/PickupBase.h"

#include "Components/CapsuleComponent.h"
#include "Core/Player/SurvivalCharacter.h"
#include "Net/UnrealNetwork.h"


APickupBase::APickupBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupMesh->SetGenerateOverlapEvents(false);
	SetRootComponent(PickupMesh);

	// Collision preset for trigger is set in BP
	TriggerCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerCollider"));
	TriggerCollider->SetupAttachment(RootComponent);
}

void APickupBase::BeginPlay()
{
	Super::BeginPlay();

	TriggerCollider->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	OnRep_ActiveState(); // Making sure the visuals are correct initially
}

void APickupBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APickupBase, bIsActive);
}

void APickupBase::OnBeginOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (!HasAuthority() || !bIsActive) return;
	
	if (ASurvivalCharacter* Player = Cast<ASurvivalCharacter>(OtherActor))
	{
		if (TryHandlePickup(Player))
		{
			DectivatePickup();
		}
	}
}

void APickupBase::ActivatePickup()
{
	bIsActive = true;
	MC_SetPickupActive(true);
}

void APickupBase::DectivatePickup()
{
	bIsActive = false;
	MC_SetPickupActive(false);

	GetWorld()->GetTimerManager().SetTimer(
		RespawnTimerHandle,
		this,
		&APickupBase::ActivatePickup,
		RespawnTime,
		false
	);
}

void APickupBase::OnRep_ActiveState()
{
	SetActorHiddenInGame(!bIsActive);
	TriggerCollider->SetCollisionEnabled(bIsActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void APickupBase::MC_SetPickupActive_Implementation(bool bNewActive)
{
	bIsActive = bNewActive;
	// Needs to be called manually so that the Server also has the state updated correctly
	OnRep_ActiveState();
}
