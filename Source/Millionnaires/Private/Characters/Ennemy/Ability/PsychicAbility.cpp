// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Psychic ability, causes the player sight to downgrade etc.
 */

#include "Characters/Ennemy/Ability/PsychicAbility.h"
#include "Interfaces/EffectInterface.h"

void UPsychicAbility::ExecuteAbility_Implementation(AActor* Owner, AActor* Target, const UAbilityDataAsset* Data)
{
	Super::ExecuteAbility_Implementation(Owner, Target, Data);

	if (Target && Target->GetClass()->ImplementsInterface(UEffectInterface::StaticClass()))
	{
		IEffectInterface::Execute_SanityEffect(Target);
	}
}
