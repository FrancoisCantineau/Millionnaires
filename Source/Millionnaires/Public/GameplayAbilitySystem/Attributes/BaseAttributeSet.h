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
};

UCLASS()
class MILLIONNAIRES_API UWeaponAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category="Weapon|Ammo")
	FGameplayAttributeData AmmoInMag;
	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, AmmoInMag)

	UPROPERTY(BlueprintReadOnly, Category="Weapon|Ammo")
	FGameplayAttributeData MaxAmmoInMag;
	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, MaxAmmoInMag)

	UPROPERTY(BlueprintReadOnly, Category="Weapon|Ammo")
	FGameplayAttributeData AmmoReserve;
	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, AmmoReserve)

	UPROPERTY(BlueprintReadOnly, Category="Weapon|Ammo")
	FGameplayAttributeData MaxAmmoReserve;
	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, MaxAmmoReserve)
};

