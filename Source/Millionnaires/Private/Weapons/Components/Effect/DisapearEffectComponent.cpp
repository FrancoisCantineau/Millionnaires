// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Implements the logic for disapearing attack effects.
 */

#include "Weapons/Components/Effect/DisapearEffectComponent.h"

void UDisapearEffectComponent::ApplyEffect(const FHitResult& Hit, AActor* Instigator)
{
	if (AActor* HitActor =Hit.GetActor())
	{
		HitActor->Destroy();
	}
}
