#include "ConsumableEffect/BatteryEffect.h"

#include "Components/FlashlightEquipmentComponent.h"

UBatteryEffect::UBatteryEffect()
{
    EffectDescription = FText::FromString("Recharges flashlight battery");
}

/*
 * Check if the effect can be applied in the given context
 */
bool UBatteryEffect::CanApplyEffect_Implementation(const FConsumableContext& Context) const
{
    if (!Context.Target)
    {
        return false;
    }

    UFlashlightEquipmentComponent* Flashlight = Context.Target->FindComponentByClass<UFlashlightEquipmentComponent>();
    if (!Flashlight)
    {
        return false;
    }

    if (!Flashlight->IsFlashlightEquipped())
    {
        return false;
    }

    return Flashlight->GetBatteryPercentage() < 100.0f;
}

/*
 * Apply the battery recharge effect
 */
EConsumableResult UBatteryEffect::ApplyEffect_Implementation(const FConsumableContext& Context)
{
    if (!Context.Target)
    {
        return EConsumableResult::Failed_NoTarget;
    }

    UFlashlightEquipmentComponent* Flashlight = Context.Target->FindComponentByClass<UFlashlightEquipmentComponent>();
    if (!Flashlight)
    {
        return EConsumableResult::Failed_CannotConsume;
    }

    if (!Flashlight->IsFlashlightEquipped())
    {
        return EConsumableResult::Failed_CannotConsume;
    }

    if (Flashlight->GetBatteryPercentage() >= 100.0f)
    {
        return EConsumableResult::Failed_AlreadyFull;
    }

    Flashlight->RechargeBattery(ChargeAmount, bFullRecharge);
    
    return EConsumableResult::Success;
}