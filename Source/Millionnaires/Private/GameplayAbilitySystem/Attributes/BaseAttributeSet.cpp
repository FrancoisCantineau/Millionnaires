// Fill out your copyright notice in the Description page of Project Settings.

/*
 * Millionaire Project, 2026
 * Created by:  "Francki"
 * Last Updated by: "Francki"
 * Class: "BaseAttributeSet" - Source
 * Notes: Hold attributes, the basic ones, for the GAS system mainly
 */


#include "GameplayAbilitySystem/Attributes/BaseAttributeSet.h"
#include "GameplayEffectExtension.h"


#pragma region BaseAttributeSet

void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UBaseAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		if (!ASC) return;
		SetHealth(GetHealth());

		FGameplayTag LowHealthTag = FGameplayTag::RequestGameplayTag(FName("State.Health.Low"));
		float HealthPercent = GetHealth() / GetMaxHealth();
		
		if (HealthPercent <= 0.30f) {
			ASC->AddLooseGameplayTag(LowHealthTag);
			
		} else {
			ASC->RemoveLooseGameplayTag(LowHealthTag);
		}

		
		FGameplayTagContainer HitReactionTagContainer;
		HitReactionTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayAbility.HitReaction")));
		GetOwningAbilitySystemComponent()->TryActivateAbilitiesByTag(HitReactionTagContainer);
	}
}

void UBaseAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetHealthAttribute() && NewValue <= 0.f)
	{
		FGameplayTagContainer DeathAbilityTagContainer;
		DeathAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Death")));
		GetOwningAbilitySystemComponent()->TryActivateAbilitiesByTag(DeathAbilityTagContainer);
	}
}
#pragma endregion BaseAttributeSet

