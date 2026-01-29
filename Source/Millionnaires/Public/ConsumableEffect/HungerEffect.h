#pragma once

#include "CoreMinimal.h"
#include "Consumable/ConsumableEffect.h"

#include "HungerEffect.generated.h"

/**
 * Consumable effect that restores hunger
 */
UCLASS(Blueprintable, meta = (DisplayName = "Hunger Restore Effect"))
class MILLIONNAIRES_API UHungerEffect : public UConsumableEffect
{
	GENERATED_BODY()

public:

	UHungerEffect();

	/** Amount of hunger to restore */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hunger", meta = (ClampMin = "0.0"))
	float HungerAmount = 30.0f;

	/** If true, hunger amount is a percentage of max hunger instead of flat value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hunger")
	bool bUsePercentage = false;
	
	virtual bool CanApplyEffect_Implementation(const FConsumableContext& Context) const override;
	virtual EConsumableResult ApplyEffect_Implementation(const FConsumableContext& Context) override;
};