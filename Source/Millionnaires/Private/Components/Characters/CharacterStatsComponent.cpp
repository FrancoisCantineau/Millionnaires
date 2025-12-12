/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "CharacterStatsComponent" - Source
 * Notes: Actor component handling health, hunger and basic character data, driven by CharacterDefinition.
 */

#include "Components/Characters/CharacterStatsComponent.h"

#include "Data/CharacterDefinition.h"
#include "Engine/Texture2D.h"
#include "Millionnaires.h"

UCharacterStatsComponent::UCharacterStatsComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterStatsComponent::BeginPlay()
{
    Super::BeginPlay();

    if (bAutoInitializeFromDefinition && CharacterDefinition)
    {
        InitializeFromDefinition(CharacterDefinition);
    }
    else
    {
        CurrentHealth = bUseHealth ? MaxHealth : 0.0f;
        CurrentHunger = 0.0f;
        ClampStats();

        if (bDebug)
        {
            UE_LOG(LogMillionnaires, Log, TEXT("[StatsComponent] '%s' initialized without data asset. Health=%.2f Hunger=%.2f"),
                *GetOwner()->GetName(), CurrentHealth, CurrentHunger);
        }
    }
}

bool UCharacterStatsComponent::IsDead() const
{
    return bUseHealth && CurrentHealth <= 0.0f;
}

void UCharacterStatsComponent::InitializeFromDefinition(UCharacterDefinition* NewDefinition)
{
    if (!NewDefinition)
    {
        if (bDebug)
        {
            UE_LOG(LogMillionnaires, Warning, TEXT("[StatsComponent] InitializeFromDefinition called with null definition on '%s'."),
                *GetOwner()->GetName());
        }
        return;
    }

    CharacterDefinition = NewDefinition;
    ApplyDefinition(NewDefinition);
    ClampStats();

    if (bDebug)
    {
        UE_LOG(LogMillionnaires, Log, TEXT("[StatsComponent] '%s' initialized from definition '%s'. MaxHealth=%.2f MaxHunger=%.2f"),
            *GetOwner()->GetName(),
            *NewDefinition->GetDisplayName().ToString(),
            MaxHealth,
            MaxHunger);
    }

    if (bUseHealth)
    {
        OnHealthChanged.Broadcast(CurrentHealth);
    }

    if (bUseHunger)
    {
        OnHungerChanged.Broadcast(CurrentHunger);
    }
}

void UCharacterStatsComponent::ModifyHealth(float Delta)
{
    if (!bUseHealth)
    {
        return;
    }

    const float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth + Delta, 0.0f, MaxHealth);

    if (!FMath::IsNearlyEqual(PreviousHealth, CurrentHealth))
    {
        if (bDebug)
        {
            UE_LOG(LogMillionnaires, Log, TEXT("[StatsComponent] '%s' health changed: %.2f -> %.2f"),
                *GetOwner()->GetName(), PreviousHealth, CurrentHealth);
        }

        OnHealthChanged.Broadcast(CurrentHealth);
    }

    if (PreviousHealth > 0.0f && CurrentHealth <= 0.0f)
    {
        if (bDebug)
        {
            UE_LOG(LogMillionnaires, Log, TEXT("[StatsComponent] '%s' died."), *GetOwner()->GetName());
        }

        OnDeath.Broadcast();
    }
}

void UCharacterStatsComponent::ModifyHunger(float Delta)
{
    if (!bUseHunger)
    {
        return;
    }

    const float PreviousHunger = CurrentHunger;
    CurrentHunger = FMath::Clamp(CurrentHunger + Delta, 0.0f, MaxHunger);

    if (!FMath::IsNearlyEqual(PreviousHunger, CurrentHunger))
    {
        if (bDebug)
        {
            UE_LOG(LogMillionnaires, Log, TEXT("[StatsComponent] '%s' hunger changed: %.2f -> %.2f"),
                *GetOwner()->GetName(), PreviousHunger, CurrentHunger);
        }

        OnHungerChanged.Broadcast(CurrentHunger);
    }
}

void UCharacterStatsComponent::ApplyDefinition(UCharacterDefinition* DefinitionToApply)
{
    if (!DefinitionToApply)
    {
        return;
    }

    CachedDisplayName = DefinitionToApply->GetDisplayName();
    Portrait = DefinitionToApply->GetPortrait();

    if (bUseHealth)
    {
        MaxHealth = DefinitionToApply->GetMaxHealth();
        CurrentHealth = MaxHealth;
    }

    if (bUseHunger)
    {
        MaxHunger = DefinitionToApply->GetMaxHunger();
        CurrentHunger = 0.0f;
    }
}

void UCharacterStatsComponent::ClampStats()
{
    if (bUseHealth)
    {
        MaxHealth = FMath::Max(0.0f, MaxHealth);
        CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
    }
    else
    {
        MaxHealth = 0.0f;
        CurrentHealth = 0.0f;
    }

    if (bUseHunger)
    {
        MaxHunger = FMath::Max(0.0f, MaxHunger);
        CurrentHunger = FMath::Clamp(CurrentHunger, 0.0f, MaxHunger);
    }
    else
    {
        MaxHunger = 0.0f;
        CurrentHunger = 0.0f;
    }
}
