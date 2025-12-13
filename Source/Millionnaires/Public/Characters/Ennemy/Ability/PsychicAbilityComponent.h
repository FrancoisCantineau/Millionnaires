// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Ennemy/Ability/AbilityBaseComponent.h"
#include "PsychicAbilityComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UPsychicAbilityComponent : public UAbilityBaseComponent
{
	GENERATED_BODY()

public :

	virtual bool UseAbility() override;
	
	FTimerHandle CooldownTimerHandle;

	
};
