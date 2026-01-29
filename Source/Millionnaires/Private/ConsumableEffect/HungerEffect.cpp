#include "ConsumableEffect/HungerEffect.h"

#include "GameplayTagContainer.h"
#include "Components/Characters/CharacterStatsComponent.h"


UHungerEffect::UHungerEffect()
{
	EffectTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Consumable.Food")));
	EffectDescription = FText::FromString("Restores hunger");
}

/*
 * Check if the hunger effect can be applied
 */
bool UHungerEffect::CanApplyEffect_Implementation(const FConsumableContext& Context) const
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

	if (StatsComp->GetCurrentHunger() >= StatsComp->GetMaxHunger())
	{
		return false;
	}

	return true;
}

/*
 * Apply the hunger restoration effect
 */
EConsumableResult UHungerEffect::ApplyEffect_Implementation(const FConsumableContext& Context)
{
	UCharacterStatsComponent* StatsComp = Context.Target->FindComponentByClass<UCharacterStatsComponent>();
    
	if (!StatsComp)
	{
		return EConsumableResult::Failed_NoTarget;
	}

	if (StatsComp->GetCurrentHunger() >= StatsComp->GetMaxHunger())
	{
		return EConsumableResult::Failed_AlreadyFull;
	}

	float RestoreAmount = HungerAmount;
	if (bUsePercentage)
	{
		RestoreAmount = StatsComp->GetMaxHunger() * (HungerAmount / 100.0f);
	}

	StatsComp->ModifyHunger(RestoreAmount);

	return EConsumableResult::Success;
}