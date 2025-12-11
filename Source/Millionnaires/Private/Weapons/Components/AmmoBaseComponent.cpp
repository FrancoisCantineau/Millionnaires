// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Components/AmmoBaseComponent.h"

#include "Kismet/GameplayStatics.h"

UAmmoBaseComponent::UAmmoBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

bool UAmmoBaseComponent::CanConsume() const
{
	return !bIsReloading && CurrentMagazine >= AmmoPerShot;
}

bool UAmmoBaseComponent::Consume()
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
    
	OnAmmoChanged.Broadcast();
    
	// ⭐ EVENT si vide
	if (CurrentMagazine == 0)
	{
		OnAmmoEmpty.Broadcast();
        
		if (bAutoReload && CanReload())
		{
			Reload();
		}
	}
    
	UE_LOG(LogTemp, Log, TEXT("Ammo: %d / %d (Reserve: %d)"), 
		   CurrentMagazine, MaxMagazineSize, CurrentReserveAmmo);

	return true;
}

void UAmmoBaseComponent::Reload()
{
	if (!CanReload())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot reload!"));
		return;
	}
    
	bIsReloading = true;
    
	// SFX
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
	// ⭐ EVENT pour animation
	OnReloadStarted.Broadcast();
    
	// ⭐ TIMER pour compléter le reload
	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&UMagazineAmmoComponent::CompleteReload,
		ReloadTime,
		false
	);
    
	UE_LOG(LogTemp, Log, TEXT("Reloading... (%.1fs)"), ReloadTime);*/
}

bool UAmmoBaseComponent::CanReload()
{
	if (bIsReloading)
		return false;
    
	// Chargeur pas plein
	if (CurrentMagazine >= MaxMagazineSize)
		return false;
    
	// A des munitions en réserve (ou réserve infinie)
	if (MaxReserveAmmo != -1 && CurrentReserveAmmo <= 0)
		return false;
    
	return true;
}

void UAmmoBaseComponent::CompleteReload()
{
	bIsReloading = false;
    
	int32 AmmoNeeded = MaxMagazineSize - CurrentMagazine;
    
	if (MaxReserveAmmo == -1)
	{
		// ⭐ RÉSERVE INFINIE
		CurrentMagazine = MaxMagazineSize;
	}
	else
	{
		// ⭐ RÉSERVE LIMITÉE
		int32 AmmoToReload = FMath::Min(AmmoNeeded, CurrentReserveAmmo);
		CurrentMagazine += AmmoToReload;
		CurrentReserveAmmo -= AmmoToReload;
	}
    
	OnReloadCompleted.Broadcast();
	OnAmmoChanged.Broadcast();
    
	UE_LOG(LogTemp, Log, TEXT("Reload complete! Ammo: %d / %d (Reserve: %d)"), 
		   CurrentMagazine, MaxMagazineSize, CurrentReserveAmmo);
}

void UAmmoBaseComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerWeapon = Cast<AWeaponBase>(GetOwner());
	
}



void UAmmoBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}



