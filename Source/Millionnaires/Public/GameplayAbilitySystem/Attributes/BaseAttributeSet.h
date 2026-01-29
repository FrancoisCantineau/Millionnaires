// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName,PropertyName)\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName,PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class MILLIONNAIRES_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	public:
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Health);
	
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Damage);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData DamageMultiplier;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, DamageMultiplier)

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData BonusDamage;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, BonusDamage)

protected:

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
};

