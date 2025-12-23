#include "ConsumableEffect/BatteryEffect.h"

#include "GameplayTagContainer.h"

UBatteryEffect::UBatteryEffect()
{
    EffectTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Consumable.Battery")));
    EffectDescription = FText::FromString("Recharges flashlight battery");
}

/*
 * Check if the effect can be applied in the given context
 */
bool UBatteryEffect::CanApplyEffect_Implementation(const FConsumableContext& Context) const
{
    if (!Super::CanApplyEffect_Implementation(Context))
    {
        return false;
    }

    // TODO: Gerer la logique lorsque le FlashlightComponent est disponible
    
    return true;
}

/*
 * Apply the battery recharge effect
 */
EConsumableResult UBatteryEffect::ApplyEffect_Implementation(const FConsumableContext& Context)
{
    // TODO: Gerer la logique lorsque le FlashlightComponent est disponible
    
    return EConsumableResult::Success;
}