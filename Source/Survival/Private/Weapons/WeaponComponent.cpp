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

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	WeaponInventory.OwnerComponent = this;
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
	if (!HasWeaponEquipped() || GetCurrentWeaponAmmo() <= 0) return;

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

	if (GetCurrentWeaponAmmo() > 0)
	{
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

void UWeaponComponent::ConsumeCurrentWeaponAmmo(uint16 AmountToConsume)
{
	if (!GetOwner()->HasAuthority() || !WeaponInventory.Items.IsValidIndex(CurrentWeaponIndex)) return;

	FWeaponInventoryItem& Item = WeaponInventory.Items[CurrentWeaponIndex];
	uint16& Ammo = Item.AmmoCount;
	// Clamp in that way instead of FMath::Max is needed as we're dealing w/ unsigned int
	Ammo = (Ammo >= AmountToConsume) ? Ammo - AmountToConsume : 0;
	WeaponInventory.MarkItemDirty(Item);

	UE_LOG(LogWeaponComponent, Warning, TEXT("Consumed %d ammo of weapon %s"), AmountToConsume, *Item.WeaponTag.ToString());
	
	// Notify UI
	OnRep_WeaponInventory();
}

void UWeaponComponent::AddAmmo(FGameplayTag WeaponTag, uint16 AmountToAdd)
{
	if (!GetOwner()->HasAuthority()) return;

	for (FWeaponInventoryItem& Item : WeaponInventory.Items)
	{
		if (Item.WeaponTag == WeaponTag)
		{
			Item.AmmoCount += AmountToAdd;
			WeaponInventory.MarkItemDirty(Item);
			return;
		}
	}

	// In case this is the first ever pickup of this ammo type
	FWeaponInventoryItem NewItem;
	NewItem.WeaponTag = WeaponTag;
	NewItem.bWeaponCollected = false;
	NewItem.AmmoCount = AmountToAdd;

	WeaponInventory.Items.Add(NewItem);
	WeaponInventory.MarkItemDirty(NewItem);

	UE_LOG(LogWeaponComponent, Warning, TEXT("Added %d ammo for weapon %s"), AmountToAdd, *WeaponTag.ToString());

	// Notify UI
	OnRep_WeaponInventory();
}

uint16 UWeaponComponent::GetCurrentWeaponAmmo() const
{
	// No write access to ammo client-side to prevent cheating
	if (WeaponInventory.Items.IsValidIndex(CurrentWeaponIndex))
	{
		return WeaponInventory.Items[CurrentWeaponIndex].AmmoCount;
	}
	else return 0;
}

void UWeaponComponent::AddNewWeapon(FGameplayTag NewWeaponTag)
{
	UE_LOG(LogWeaponComponent, Warning, TEXT("Weapon picked up on %s"), *GetOwner()->GetName());
	
	if (auto* MatchingEntry = WeaponInventory.Items.FindByPredicate(
		[NewWeaponTag](FWeaponInventoryItem& Entry)
		{
			return Entry.WeaponTag == NewWeaponTag;
		}))
	{
		if (!MatchingEntry->bWeaponCollected)
		{
			MatchingEntry->bWeaponCollected = true;
			AddWeaponProp(NewWeaponTag);
			WeaponInventory.MarkItemDirty(*MatchingEntry);
		}
	}
	else
	{
		auto Item = FWeaponInventoryItem();
		Item.WeaponTag = NewWeaponTag;
		Item.bWeaponCollected = true;
		Item.AmmoCount = 0;
		WeaponInventory.Items.Add(Item);
		WeaponInventory.MarkItemDirty(Item);
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
	
	if (WeaponInventory.Items.Num() <= 1) return;

	EquippedWeaponProp->SetActorHiddenInGame(true);
	CurrentWeaponIndex += 1;
	if (CurrentWeaponIndex >= WeaponInventory.Items.Num())
	{
		CurrentWeaponIndex = 0;
	}

	ensure(Database->TryGetWeaponData(WeaponInventory.Items[CurrentWeaponIndex].WeaponTag,CurrentWeaponData));
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

	ConsumeCurrentWeaponAmmo(1);
	MC_FireEffects();

	// With animations working correctly, I'd take the muzzle socket transform from the currently held weapon here
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

void UWeaponComponent::OnRep_WeaponInventory() const
{
	OnAmmoChanged.Broadcast(GetCurrentWeaponAmmo());
}

void UWeaponComponent::OnRep_EquippedWeaponProp()
{
	if (!EquippedWeaponProp) return;

	AttachPropToPlayer(EquippedWeaponProp);
	EquippedWeaponProp->SetActorHiddenInGame(false);

	// Calling this to trigger ammo UI update on weapon change
	OnRep_WeaponInventory();
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
