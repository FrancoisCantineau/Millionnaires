// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Implements the logic for attack effects, as fire, ice etc etc etc.
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponEffectBaseComponent.generated.h"


UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UWeaponEffectBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UWeaponEffectBaseComponent();

	UFUNCTION()
	virtual void ApplyEffect(const FHitResult& Hit, AActor* Instigator);
protected:
	
	virtual void BeginPlay() override;

	

	
public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
