// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ennemy/Ability/Conditions/HealthTresholdCondition.h"

#include "Components/Characters/CharacterStatsComponent.h"

bool UHealthTresholdCondition::CheckCondition_Implementation(AActor* Owner) const
{
	if (!Owner)
	{
		return false;
	}
	float CurrentHealthPercent = GetHealthPercent(Owner);
	
	return CurrentHealthPercent >= MinHealthPercent && 
		   CurrentHealthPercent <= MaxHealthPercent;
}

float UHealthTresholdCondition::GetHealthPercent(AActor* Owner) const
{
	if (!Owner)
	{
		return 0.0f;
	}
    

	UCharacterStatsComponent* StatsComp = Owner->FindComponentByClass<UCharacterStatsComponent>();
    
	if (!StatsComp)
	{
		return 0.0f;
	}
    
	float CurrentHealth = StatsComp->GetCurrentHealth();
	float MaxHealth = StatsComp->GetMaxHealth();
	
	if (MaxHealth <= 0.0f)
	{
		return 0.0f;
	}
	
	return (CurrentHealth / MaxHealth) * 100.0f;
}
