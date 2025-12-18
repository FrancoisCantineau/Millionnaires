// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Psychic ability, causes the player sight to downgrade etc.
 */

#include "Characters/Ennemy/Ability/PsychicAbilityComponent.h"

void UPsychicAbilityComponent::ExecuteAbility(AActor* Target)
{
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "PsychicAbilityComponent");
	
}
