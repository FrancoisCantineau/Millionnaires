#pragma once

#include "CoreMinimal.h"
#include "ConsumableInterface.h"
#include "GameplayTagContainer.h"

#include "ConsumableEffect.generated.h"

/**
 * Base class for consumable effects
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class GAMEINTERFACES_API UConsumableEffect : public UObject
{
	GENERATED_BODY()

public:

	/** Gameplay tags identifying this effect type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	FGameplayTagContainer EffectTags;

	/** Description of what this effect does */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	FText EffectDescription;
	
	/** Check if this effect can be applied in the given context */
	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
	bool CanApplyEffect(const FConsumableContext& Context) const;
	virtual bool CanApplyEffect_Implementation(const FConsumableContext& Context) const;

	/** Apply the effect */
	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
	EConsumableResult ApplyEffect(const FConsumableContext& Context);
	virtual EConsumableResult ApplyEffect_Implementation(const FConsumableContext& Context);

	/** Get the effect tags */
	FORCEINLINE FGameplayTagContainer GetEffectTags() const { return EffectTags; }

	/** Get the effect description */
	FORCEINLINE FText GetEffectDescription() const { return EffectDescription; }
};