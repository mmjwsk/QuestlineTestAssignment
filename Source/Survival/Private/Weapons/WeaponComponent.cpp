// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponComponent.h"

#include "Core/Player/SurvivalCharacter.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/Projectile.h"
#include "Weapons/WeaponDatabase.h"
#include "Weapons/WeaponProp.h"

UWeaponComponent::UWeaponComponent()
{
	SetIsReplicatedByDefault(true);
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, WeaponInventory);
	DOREPLIFETIME(UWeaponComponent, EquippedWeaponProp);
	DOREPLIFETIME(UWeaponComponent, WeaponProps);
	DOREPLIFETIME(UWeaponComponent, bIsFiring);
}

void UWeaponComponent::StartFire()
{
	if (!HasWeaponEquipped() || GetAmmo() <= 0) return;

	if (!GetOwner()->HasAuthority())
	{
		S_Fire();
	}

	bIsFiring = true;
	HandleFiring();
}

void UWeaponComponent::StopFire()
{
	bIsFiring = false;
	GetWorld()->GetTimerManager().ClearTimer(FireRateTimerHandle);
}

void UWeaponComponent::HandleFiring()
{
	Fire();

	if (CurrentWeaponData.bIsAutomatic && bIsFiring)
	{
		GetWorld()->GetTimerManager().SetTimer(
			FireRateTimerHandle,
			this,
			&UWeaponComponent::HandleFiring,
			CurrentWeaponData.RateOfFire,
			false
		);
	}
}

void UWeaponComponent::AddWeaponProp(FGameplayTag WeaponTag)
{
	if (!GetOwner()->HasAuthority()) return;

	FWeaponData WeaponData;
	if (Database->TryGetWeaponData(WeaponTag, WeaponData))
	{
		if (auto* WeaponPropClass = WeaponData.WeaponProp.LoadSynchronous())
		{
			FActorSpawnParameters Params;
			Params.Owner = GetOwner();
			Params.Instigator = Cast<APawn>(GetOwner());
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			if (AWeaponProp* NewWeapon = GetWorld()->SpawnActor<AWeaponProp>(WeaponPropClass, Params))
			{
				NewWeapon->SetOwner(GetOwner());
				NewWeapon->SetActorHiddenInGame(true); // hide until equipped
				NewWeapon->SetReplicates(true);
				WeaponProps.Add(NewWeapon);
			}
		}
	}
}

void UWeaponComponent::AttachPropToPlayer(AWeaponProp* Prop)
{
	ASurvivalCharacter* OwnerChar = Cast<ASurvivalCharacter>(GetOwner());
	if (!OwnerChar || !Prop) return;

	const FAttachmentTransformRules AttachmentRule(EAttachmentRule::SnapToTarget, false);

	// attach the weapon actor
	Prop->AttachToActor(OwnerChar, AttachmentRule);
	
	// Attach visible meshes safely
	SafeAttachToSocket(Prop->GetFPPMesh(), OwnerChar->GetFirstPersonMesh(), FirstPersonWeaponSocket, AttachmentRule, 0.05f);
	SafeAttachToSocket(Prop->GetTPPMesh(), OwnerChar->GetMesh(), ThirdPersonWeaponSocket, AttachmentRule, 0.05f);
}

void UWeaponComponent::AddNewWeapon(FGameplayTag NewWeaponTag)
{
	if (auto* MatchingEntry = WeaponInventory.FindByPredicate(
		[NewWeaponTag](FWeaponInventoryData& Entry)
		{
			return Entry.WeaponTag == NewWeaponTag;
		}))
	{
		if (!MatchingEntry->bWeaponCollected)
		{
			MatchingEntry->bWeaponCollected = true;
			AddWeaponProp(NewWeaponTag);
		}
	}
	else
	{
		WeaponInventory.Add(FWeaponInventoryData{NewWeaponTag, true, 0});
		AddWeaponProp(NewWeaponTag);
	}

	if (!HasWeaponEquipped())
	{
		CurrentWeaponIndex = 0;
		ensure(Database->TryGetWeaponData(NewWeaponTag,CurrentWeaponData));
		if (WeaponProps.IsValidIndex(CurrentWeaponIndex))
		{
			EquippedWeaponProp = WeaponProps[CurrentWeaponIndex];
			OnRep_EquippedWeaponProp();
		}
	}
}

void UWeaponComponent::Fire()
{
	if (!GetOwner()->HasAuthority()) return;

	GetAmmo()--;
	MC_FireEffects();

	FVector MuzzleLocation = FVector::Zero(); //WeaponMesh->GetSocketLocation("Muzzle");
	FRotator MuzzleRotation = FRotator::ZeroRotator; //WeaponMesh->GetSocketRotation("Muzzle");

	if (auto* ProjectileClass = CurrentWeaponData.ProjectileClass.LoadSynchronous())
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);
		// Spawn deferred and insert projectile data here
	}
}

void UWeaponComponent::S_Fire_Implementation()
{
	HandleFiring();
}

void UWeaponComponent::MC_FireEffects_Implementation()
{
	// VFX & SFX to make firing pretty go here
}

void UWeaponComponent::OnRep_WeaponInventory()
{
	// Update UI or reload feedback
}

void UWeaponComponent::OnRep_EquippedWeaponProp()
{
	if (!EquippedWeaponProp) return;

	AttachPropToPlayer(EquippedWeaponProp);
	EquippedWeaponProp->SetActorHiddenInGame(false);
}

void UWeaponComponent::SafeAttachToSocket(USceneComponent* Child, USceneComponent* Parent, const FName& SocketName,
	const FAttachmentTransformRules& AttachmentRules, float DelaySeconds)
{
	if (!Child || !Parent)
	{
		// Defer and try again after delay if components are not yet valid
		if (DelaySeconds > 0.0f)
		{
			FTimerHandle RetryHandle;
			GetWorld()->GetTimerManager().SetTimer(RetryHandle, FTimerDelegate::CreateLambda([&]()
			{
				SafeAttachToSocket(Child, Parent, SocketName, AttachmentRules, 0.0f);
			}), DelaySeconds, false);
		}
		return;
	}

	Child->AttachToComponent(Parent, AttachmentRules, SocketName);
}
