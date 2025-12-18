// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/WeaponBase.h"
#include "WeaponResourceComponentBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int, CurrentResourceAmount, int, MaxResourceAmount);

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UWeaponResourceComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponResourceComponentBase();

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	virtual bool CanConsume() const;
    
	// Consume ammo/durability/etc
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	virtual bool Consume();
    
	// Reload/cooldown/repair
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	virtual void Reload();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual float GetResourcePercent() const PURE_VIRTUAL(UWeaponResourceComponent::GetResourcePercent, return 1.f;);

	// Are we reloading
	UFUNCTION(BlueprintPure, Category = "Ammo")
	virtual bool IsReloading() const { return false; }
    
	// Can we reload
	UFUNCTION(BlueprintPure, Category = "Ammo")
	virtual bool CanReload();
    
	UFUNCTION(BlueprintPure, Category = "Ammo")
	virtual FString GetAmmoDisplayText() const { return TEXT("∞"); }
	
	void ChangesApplied(int Current, int Max);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	AWeaponBase* OwnerWeapon;

	UPROPERTY()
	class UAmmoWidgetBase* LinkedWidget;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FOnAmmoChanged OnResourceChanged;
		
};
