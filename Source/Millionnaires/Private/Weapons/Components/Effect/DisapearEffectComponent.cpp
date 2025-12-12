// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Components/Effect/DisapearEffectComponent.h"

void UDisapearEffectComponent::ApplyEffect(const FHitResult& Hit, AActor* Instigator)
{
	if (AActor* HitActor =Hit.GetActor())
	{
		HitActor->Destroy();
	}
}
