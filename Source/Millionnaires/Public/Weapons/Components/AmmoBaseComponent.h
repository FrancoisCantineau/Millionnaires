// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/WeaponBase.h"
#include "AmmoBaseComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UAmmoBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAmmoBaseComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	virtual bool CanConsume() const;
    
	// Consume ammo/durability/etc
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	virtual bool Consume();
    
	// Reload/cooldown/repair
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	virtual void Reload();
    
	// Are we reloading
	UFUNCTION(BlueprintPure, Category = "Ammo")
	virtual bool IsReloading() const { return false; }
    
	// Can we reload
	UFUNCTION(BlueprintPure, Category = "Ammo")
	virtual bool CanReload();
	
	UFUNCTION(BlueprintPure, Category = "Ammo")
	virtual float GetAmmoPercentage() const { return 1.f; }
    
	UFUNCTION(BlueprintPure, Category = "Ammo")
	virtual FString GetAmmoDisplayText() const { return TEXT("∞"); }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmmoChanged);
	UPROPERTY(BlueprintAssignable, Category = "Ammo")
	FOnAmmoChanged OnAmmoChanged;
    
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadStarted);
	UPROPERTY(BlueprintAssignable, Category = "Ammo")
	FOnReloadStarted OnReloadStarted;
    
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadCompleted);
	UPROPERTY(BlueprintAssignable, Category = "Ammo")
	FOnReloadCompleted OnReloadCompleted;
    
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmmoEmpty);
	UPROPERTY(BlueprintAssignable, Category = "Ammo")
	FOnAmmoEmpty OnAmmoEmpty;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo|Magazine")
	int32 MaxMagazineSize = 30;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo|Magazine")
	int32 CurrentMagazine = 30;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo|Reserve")
	int32 MaxReserveAmmo = 120;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo|Reserve")
	int32 CurrentReserveAmmo = 120;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY()
	AWeaponBase* OwnerWeapon;

	bool bIsReloading = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	bool bAutoReload = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	USoundBase* EmptySound;

	UPROPERTY(EditAnywhere, Category = "Ammo|SFX")
	USoundBase* ReloadSound;

	void CompleteReload();
	
	UPROPERTY(EditAnywhere, Category = "Ammo|Consume")
	int32 AmmoPerShot = 1;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
