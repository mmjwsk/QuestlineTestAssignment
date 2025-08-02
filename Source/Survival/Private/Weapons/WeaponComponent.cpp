// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponComponent.h"

#include "Components/CapsuleComponent.h"
#include "Core/Player/SurvivalCharacter.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/FakeMuzzle.h"
#include "Weapons/Projectile.h"
#include "Weapons/WeaponDatabase.h"
#include "Weapons/WeaponProp.h"

DEFINE_LOG_CATEGORY(LogWeaponComponent)

UWeaponComponent::UWeaponComponent()
{
	SetIsReplicatedByDefault(true);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UWeaponComponent, WeaponInventory);
	DOREPLIFETIME(UWeaponComponent, CurrentWeaponIndex);
	DOREPLIFETIME(UWeaponComponent, EquippedWeaponProp);
	DOREPLIFETIME(UWeaponComponent, WeaponProps);
	DOREPLIFETIME(UWeaponComponent, bIsFiring);
}

void UWeaponComponent::StartFire()
{
	//if (!HasWeaponEquipped() || GetAmmo() <= 0) return;
	if (!HasWeaponEquipped()) return;

	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogWeaponComponent, Warning, TEXT("[CLIENT] Calling S_Fire()"));
		S_Fire();
	}
	else
	{
		bIsFiring = true;
		HandleFiring();
	}
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
	UE_LOG(LogWeaponComponent, Warning, TEXT("Weapon picked up on %s"), *GetOwner()->GetName());
	
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

void UWeaponComponent::ScrollWeapon()
{
	if (!GetOwner()->HasAuthority())
	{
		S_ScrollWeapon();
		return;
	}
	
	if (WeaponInventory.Num() <= 1) return;

	EquippedWeaponProp->SetActorHiddenInGame(true);
	CurrentWeaponIndex += 1;
	if (CurrentWeaponIndex >= WeaponInventory.Num())
	{
		CurrentWeaponIndex = 0;
	}

	ensure(Database->TryGetWeaponData(WeaponInventory[CurrentWeaponIndex].WeaponTag,CurrentWeaponData));
	EquippedWeaponProp = WeaponProps[CurrentWeaponIndex];
	OnRep_EquippedWeaponProp();
}

void UWeaponComponent::S_ScrollWeapon_Implementation()
{
	ScrollWeapon();
}

void UWeaponComponent::Fire()
{
	UE_LOG(LogWeaponComponent, Warning, TEXT("Fire() called on %s"), *GetOwner()->GetName());
	
	if (!GetOwner()->HasAuthority()) return;

	GetAmmo()--;
	MC_FireEffects();

	// With animations working correctly I'd take the muzzle socket transform from the currently held weapon here
	FTransform MuzzleLocation = GetOwner()->FindComponentByClass<UFakeMuzzle>()->GetComponentTransform();

	if (auto* ProjectileClass = CurrentWeaponData.ProjectileClass.LoadSynchronous())
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (auto* ProjectileActor = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, MuzzleLocation, OwnerCharacter,
			OwnerCharacter, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, ESpawnActorScaleMethod::MultiplyWithRoot))
		{
			// So we don't shoot ourselves
			OwnerCharacter->GetCapsuleComponent()->IgnoreActorWhenMoving(ProjectileActor, true);
			ProjectileActor->SetProperties(CurrentWeaponData.Damage, CurrentWeaponData.ProjectileMuzzleSpeed, CurrentWeaponData.ProjectileLifeSpan);
			ProjectileActor->FinishSpawning(MuzzleLocation);
		}
	}
}

void UWeaponComponent::S_Fire_Implementation()
{
	UE_LOG(LogWeaponComponent, Warning, TEXT("S_Fire_Implementation() called on server"));

	bIsFiring = true;
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
