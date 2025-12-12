// Fill out your copyright notice in the Description page of Project Settings.


/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Implements the logic for buffs/debuffs of the weapon.
 */

#include "Weapons/Components/WeaponBuffComponent.h"

// Sets default values for this component's properties
UWeaponBuffComponent::UWeaponBuffComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponBuffComponent::BeginPlay()
{
    Super::BeginPlay();
    BuildAttributeMap();
}

void UWeaponBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    CleanupExpiredModifiers();
}

void UWeaponBuffComponent::BuildAttributeMap()
{
    AttributeMap.Empty();
    AttributeMap.Add("Damage", &Damage);
    AttributeMap.Add("FireRate", &FireRate);
    AttributeMap.Add("Range", &Range);
    AttributeMap.Add("CriticalChance", &CriticalChance);
    AttributeMap.Add("CriticalMultiplier", &CriticalMultiplier);
}

FGameplayAttribute* UWeaponBuffComponent::GetAttributeByName(FName AttributeName)
{
    FGameplayAttribute** Found = AttributeMap.Find(AttributeName);
    return Found ? *Found : nullptr;
}

void UWeaponBuffComponent::AddModifierToAttribute(FName AttributeName, const FStatModifier& Modifier)
{
    FGameplayAttribute* Attr = GetAttributeByName(AttributeName);
    if (!Attr) return;

    FStatModifier NewMod = Modifier;
    NewMod.CreationTime = GetWorld()->GetTimeSeconds();
    Attr->AddModifier(NewMod);
}

void UWeaponBuffComponent::RemoveModifierFromAttribute(FName AttributeName, FName ModifierTag)
{
    FGameplayAttribute* Attr = GetAttributeByName(AttributeName);
    if (!Attr) return;

    Attr->RemoveModifier(ModifierTag);
}

void UWeaponBuffComponent::ClearAllModifiers()
{
    for (auto& Pair : AttributeMap)
    {
        if (Pair.Value)
            Pair.Value->ClearModifiers();
    }
}

void UWeaponBuffComponent::CleanupExpiredModifiers()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    for (auto& Pair : AttributeMap)
    {
        if (Pair.Value)
            Pair.Value->CleanupExpired(CurrentTime);
    }
}

void UWeaponBuffComponent::InitializeFromData(float InDamage, float InFireRate, float InRange)
{
    Damage.BaseValue = InDamage;
    FireRate.BaseValue = InFireRate;
    Range.BaseValue = InRange;

    BuildAttributeMap();
}

