#pragma once

#include "CoreMinimal.h"
#include "Consumable/ConsumableEffect.h"

#include "HealthEffect.generated.h"

/**
 * A consumable effect that restores health to the target actor.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Health Restore Effect"))
class MILLIONNAIRES_API UHealthEffect : public UConsumableEffect
{
	GENERATED_BODY()

public:

	UHealthEffect();

	/** Amount of health to restore */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0.0"))
	float HealthAmount = 25.0f;

	/** If true, heal amount is a percentage of max health instead of flat value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bUsePercentage = false;
	
	virtual bool CanApplyEffect_Implementation(const FConsumableContext& Context) const override;
	virtual EConsumableResult ApplyEffect_Implementation(const FConsumableContext& Context) override;
};