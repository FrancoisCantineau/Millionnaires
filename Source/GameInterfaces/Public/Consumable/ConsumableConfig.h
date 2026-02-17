#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Consumable/ConsumableEffect.h"
#include "ConsumableConfig.generated.h"

/**
 * Configuration asset for consumable items
 * Reference this from your DataTable instead of inline effects
 */
UCLASS(BlueprintType)
class GAMEINTERFACES_API UConsumableConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Effects applied when consumed - Instanced works in Data Assets! */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Consumable")
	TArray<UConsumableEffect*> Effects;

	/** Should item be consumed after use? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable")
	bool bConsumeOnUse = true;

	/** Get all tags from effects */
	FGameplayTagContainer GetEffectTags() const
	{
		FGameplayTagContainer Tags;
		for (const UConsumableEffect* Effect : Effects)
		{
			if (Effect)
			{
				Tags.AppendTags(Effect->GetEffectTags());
			}
		}
		return Tags;
	}
};