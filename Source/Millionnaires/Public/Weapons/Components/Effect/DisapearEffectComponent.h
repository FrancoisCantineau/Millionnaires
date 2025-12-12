// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Components/Effect/WeaponEffectBaseComponent.h"
#include "DisapearEffectComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UDisapearEffectComponent : public UWeaponEffectBaseComponent
{
	GENERATED_BODY()
	
	virtual void ApplyEffect(const FHitResult& Hit, AActor* Instigator) override;
};
