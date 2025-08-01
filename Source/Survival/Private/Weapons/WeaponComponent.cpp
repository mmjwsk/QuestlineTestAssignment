// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/Projectile.h"

UWeaponComponent::UWeaponComponent()
{
    SetIsReplicatedByDefault(true);
}

void UWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UWeaponComponent::StartFire()
{
    if (!HasAuthority())
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
    if (GetAmmo() <= 0) return;

    Fire();

    if (CurrentWeaponData->bIsAutomatic && bIsFiring)
    {
        GetWorld()->GetTimerManager().SetTimer(
            FireRateTimerHandle,
            this,
            &UWeaponComponent::HandleFiring,
            CurrentWeaponData->RateOfFire,
            false
        );
    }
}

void UWeaponComponent::Fire()
{
    if (!HasAuthority()) return;

    GetAmmo()--;
    MC_FireEffects();

    FVector MuzzleLocation = FVector::Zero(); //WeaponMesh->GetSocketLocation("Muzzle");
    FRotator MuzzleRotation = FRotator::ZeroRotator; //WeaponMesh->GetSocketRotation("Muzzle");

    if (auto* ProjectileClass = CurrentWeaponData->ProjectileClass.LoadSynchronous())
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

bool UWeaponComponent::HasAuthority() const
{
    return IsNetMode(NM_ListenServer) || IsNetMode(NM_Standalone);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UWeaponComponent, WeaponInventory);
    DOREPLIFETIME(UWeaponComponent, bIsFiring);
}
