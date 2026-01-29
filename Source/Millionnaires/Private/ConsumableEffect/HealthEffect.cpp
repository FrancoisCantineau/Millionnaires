#include "ConsumableEffect/HealthEffect.h"
#include "GameplayTagContainer.h"
#include "Components/Characters/CharacterStatsComponent.h"

UHealthEffect::UHealthEffect()
{
    EffectTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Consumable.Health")));
    EffectDescription = FText::FromString("Restores health");
}

bool UHealthEffect::CanApplyEffect_Implementation(const FConsumableContext& Context) const
{
    if (!Super::CanApplyEffect_Implementation(Context))
    {
        return false;
    }

    UCharacterStatsComponent* StatsComp = Context.Target->FindComponentByClass<UCharacterStatsComponent>();
    if (!StatsComp)
    {
        return false;
    }

    float CurrentHealth = StatsComp->GetCurrentHealth();
    float MaxHealth = StatsComp->GetMaxHealth();
    
    if (CurrentHealth >= MaxHealth)
    {
        return false;
    }

    return true;
}

EConsumableResult UHealthEffect::ApplyEffect_Implementation(const FConsumableContext& Context)
{
    UCharacterStatsComponent* StatsComp = Context.Target->FindComponentByClass<UCharacterStatsComponent>();
    
    if (!StatsComp)
    {
        return EConsumableResult::Failed_NoTarget;
    }

    float CurrentHealth = StatsComp->GetCurrentHealth();
    float MaxHealth = StatsComp->GetMaxHealth();

    if (CurrentHealth >= MaxHealth)
    {
        return EConsumableResult::Failed_AlreadyFull;
    }

    float HealAmount = HealthAmount;
    if (bUsePercentage)
    {
        HealAmount = MaxHealth * (HealthAmount / 100.0f);
    }

    StatsComp->ModifyHealth(HealAmount);
    
    return EConsumableResult::Success;
}