// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Implements the logic for ammos management (reload and uses of ammos).
 */

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Components/Resources/WeaponResourceComponentBase.h"
#include "AmmoWeaponComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UAmmoWeaponComponent : public UWeaponResourceComponentBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAmmoWeaponComponent();

	virtual bool CanConsume() const override;
	
	virtual bool Consume() override;
	
	virtual void Reload() override;
	
	virtual float GetResourcePercent() const override { return 0; };

	virtual bool IsReloading() const override {return false;};
	
	virtual bool CanReload()  override;
	
	virtual FString GetAmmoDisplayText() const override { return TEXT("∞"); }
	
	virtual void SetCurrentAmmo(int32 mCurrentAmmo) override { CurrentMagazine = mCurrentAmmo; };
	
	virtual int GetCurrentAmmo() const override {return CurrentMagazine;}
	
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
