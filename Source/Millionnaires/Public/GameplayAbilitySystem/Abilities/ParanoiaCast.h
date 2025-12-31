// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ParanoiaCast.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UParanoiaCast : public UGameplayAbility
{
	GENERATED_BODY()


protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	
};	
