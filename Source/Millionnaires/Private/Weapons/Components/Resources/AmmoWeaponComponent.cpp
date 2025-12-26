// Fill out your copyright notice in the Description page of Project Settings.


/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Implements the logic for ammos management (reload and uses of ammos).
 */

#include "Weapons/Components/Resources/AmmoWeaponComponent.h"
#include "Weapons/WeaponBase.h"
#include "Kismet/GameplayStatics.h"

UAmmoWeaponComponent::UAmmoWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

bool UAmmoWeaponComponent::CanConsume() const
{
	return !bIsReloading && CurrentMagazine >= AmmoPerShot;
}

bool UAmmoWeaponComponent::Consume()
{
	if (!CanConsume())
	{
	
		if (CurrentMagazine == 0 && EmptySound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this, 
				EmptySound, 
				OwnerWeapon->GetActorLocation()
			);
		}
        
		
		if (bAutoReload && CurrentMagazine == 0 && CanReload())
		{
			Reload();
		}
        
		return false;
	}
    
	CurrentMagazine -= AmmoPerShot;
    
	
	if (CurrentMagazine == 0)
	{
        
		if (bAutoReload && CanReload())
		{
			Reload();
		}
	}
    
	UE_LOG(LogTemp, Log, TEXT("Ammo: %d / %d (Reserve: %d)"), 
		   CurrentMagazine, MaxMagazineSize, CurrentReserveAmmo);

	ChangesApplied(CurrentMagazine, MaxMagazineSize);
	
	return true;
}

void UAmmoWeaponComponent::Reload()
{
	if (!CanReload())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot reload!"));
		return;
	}
    
	bIsReloading = true;
    
	
	if (ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, 
			ReloadSound, 
			OwnerWeapon->GetActorLocation()
		);
	}
	CompleteReload();
  /*  
	
	OnReloadStarted.Broadcast();
    
	
	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&UMagazineAmmoComponent::CompleteReload,
		ReloadTime,
		false
	);
    
	UE_LOG(LogTemp, Log, TEXT("Reloading... (%.1fs)"), ReloadTime);*/
}

bool UAmmoWeaponComponent::CanReload()
{
	if (bIsReloading)
		return false;
    
	
	if (CurrentMagazine >= MaxMagazineSize)
		return false;
    

	if (MaxReserveAmmo != -1 && CurrentReserveAmmo <= 0)
		return false;
    
	return true;
}

void UAmmoWeaponComponent::CompleteReload()
{
	bIsReloading = false;
    
	int32 AmmoNeeded = MaxMagazineSize - CurrentMagazine;
    
	if (MaxReserveAmmo == -1)
	{
		
		CurrentMagazine = MaxMagazineSize;
	}
	else
	{
	
		int32 AmmoToReload = FMath::Min(AmmoNeeded, CurrentReserveAmmo);
		CurrentMagazine += AmmoToReload;
		CurrentReserveAmmo -= AmmoToReload;
	}
	
    
	UE_LOG(LogTemp, Log, TEXT("Reload complete! Ammo: %d / %d (Reserve: %d)"), 
		   CurrentMagazine, MaxMagazineSize, CurrentReserveAmmo);

	ChangesApplied(CurrentMagazine, MaxMagazineSize);
}

void UAmmoWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	ChangesApplied(CurrentMagazine, MaxMagazineSize);
}



void UAmmoWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}



