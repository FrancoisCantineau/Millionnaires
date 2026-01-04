// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "StatusAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName,PropertyName)\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName,PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class MILLIONNAIRES_API UStatusAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

#pragma region PsychosisStatus
	
	UPROPERTY(BlueprintReadOnly, Category="Psychosis")
	FGameplayAttributeData Psychosis;
	ATTRIBUTE_ACCESSORS(UStatusAttributeSet, Psychosis)

	UPROPERTY(BlueprintReadOnly, Category="Psychosis")
	FGameplayAttributeData MaxPsychosis;
	ATTRIBUTE_ACCESSORS(UStatusAttributeSet, MaxPsychosis)

	UPROPERTY(BlueprintReadOnly, Category="Psychosis")
	FGameplayAttributeData MentalResistance;
	ATTRIBUTE_ACCESSORS(UStatusAttributeSet, MentalResistance)

#pragma endregion PsychosisStatus

#pragma region FrozenStatus
	
	UPROPERTY(BlueprintReadOnly, Category="Freeze")
	FGameplayAttributeData FreezeStacks;
	ATTRIBUTE_ACCESSORS(UStatusAttributeSet, FreezeStacks)

	UPROPERTY(BlueprintReadOnly, Category="Freeze")
	FGameplayAttributeData FreezeResistance;
	ATTRIBUTE_ACCESSORS(UStatusAttributeSet, FreezeResistance)

#pragma endregion FrozenStatus

protected :

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
};
