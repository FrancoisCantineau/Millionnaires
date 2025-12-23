#include "Consumable/ConsumableEffect.h"

bool UConsumableEffect::CanApplyEffect_Implementation(const FConsumableContext& Context) const
{
	return Context.Target != nullptr;
}

EConsumableResult UConsumableEffect::ApplyEffect_Implementation(const FConsumableContext& Context)
{
	return EConsumableResult::Success;
}