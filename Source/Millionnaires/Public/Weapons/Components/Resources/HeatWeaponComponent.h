// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Components/Resources/WeaponResourceComponentBase.h"
#include "HeatWeaponComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UHeatWeaponComponent : public UWeaponResourceComponentBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	float MaxHeat = 100.f;

	UPROPERTY(VisibleAnywhere)
	float CurrentHeat = 0.f;

	UPROPERTY(EditAnywhere)
	float HeatPerShot = 10.f;

	UPROPERTY(EditAnywhere)
	float CoolingRate = 20.f;

	UPROPERTY(EditAnywhere)
	float OverheatThreshold = 100.f;

	UPROPERTY(EditAnywhere)
	float MinimumTimeBeforeCooling = 1.f;
	
	float LastUse = 0;
	
	virtual bool CanConsume() const override;

	virtual bool Consume() override;

	virtual float GetResourcePercent() const override;

protected:

	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
