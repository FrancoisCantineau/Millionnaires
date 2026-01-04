// Fill out your copyright notice in the Description page of Project Settings.

/*
 * Millionaire Project, 2026
 * Created by:  "Francki"
 * Last Updated by: "Francki"
 * Class: "BaseAttributeSet" - Source
 * Notes: Hold attributes, the basic ones, for the GAS system mainly
 */


#include "GameplayAbilitySystem/Attributes/StatusAttributeSet.h"
#include "GameplayEffectExtension.h"


#pragma region PsychosisAttributeSet

void UStatusAttributeSet::PreAttributeChange(
	const FGameplayAttribute& Attribute,
	float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetPsychosisAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxPsychosis());
	}
}

void UStatusAttributeSet::PostGameplayEffectExecute(
	const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetPsychosisAttribute())
	{
		SetPsychosis(GetPsychosis());

		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		if (!ASC) return;

		if (GetPsychosis() <= 0.f)
		{
			FGameplayTagContainer TagsToRemove;
			TagsToRemove.AddTag(FGameplayTag::RequestGameplayTag("State.Psychosis"));

			ASC->RemoveActiveEffectsWithGrantedTags(TagsToRemove);

		}
	}

	
}

void UStatusAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetFreezeStacksAttribute())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		FGameplayTag FrozenTag = FGameplayTag::RequestGameplayTag("State.Freeze.Frozen");
		if (!ASC) return;

		if (GetFreezeStacks() >= 5.f && !ASC->HasMatchingGameplayTag(FrozenTag))
		{
			ASC->AddLooseGameplayTag(FrozenTag);
			/*	
				FGameplayTagContainer TagsToRemove;
				TagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FrozenTag));
	
				ASC->RemoveActiveEffectsWithGrantedTags(TagsToRemove);
	*/
		}
		else if(GetFreezeStacks() < 5.f && ASC->HasMatchingGameplayTag(FrozenTag))
		{
			ASC->RemoveLooseGameplayTag(FrozenTag);
		}
	}
}
#pragma endregion PsychosisAttributeSet
