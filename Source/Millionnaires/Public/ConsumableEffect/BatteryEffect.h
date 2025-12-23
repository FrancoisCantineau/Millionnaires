#pragma once

#include "CoreMinimal.h"
#include "Consumable/ConsumableEffect.h"

#include "BatteryEffect.generated.h"

class UFlashlightComponent;

/**
 * Effect that recharges a flashlight's battery
 */
UCLASS(Blueprintable, meta = (DisplayName = "Battery Recharge Effect"))
class MILLIONNAIRES_API UBatteryEffect : public UConsumableEffect
{
	GENERATED_BODY()

public:

	UBatteryEffect();

	/** Amount of battery charge to restore (0-100) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battery", 
		meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float ChargeAmount = 50.0f;

	/** If true, fully charges the battery */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battery")
	bool bFullRecharge = false;
	
	virtual bool CanApplyEffect_Implementation(const FConsumableContext& Context) const override;
	virtual EConsumableResult ApplyEffect_Implementation(const FConsumableContext& Context) override;
};
