#pragma once

#include "CoreMinimal.h"
#include "Conditions/GameplayCondition.h"
#include "GameplayCondition_ContextTag.generated.h"

/**
 * Checks if the event context (Context.AdditionnalTags). contains a given tag.
 * Useful to distinguish variations from same tags (ex : "Event.Door.Overlapped" with an AdditionnalTags "Instigator.Player" vs "Instigator.NPC"
 */
UCLASS(meta = (DisplayName = "Context Tag Matches"))
class GAMEPLAYORCHESTRATOR_API UGameplayCondition_ContextTag : public UGameplayCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context Tag Matches")
	FGameplayTag RequiredTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context Tag Matches")
	bool bMatchChildTags = true;

	virtual bool Evaluate_Implementation(const FEventContext& Context) override
	{
		return bMatchChildTags
			? Context.AdditionalTags.HasTag(RequiredTag)
			: Context.AdditionalTags.HasTagExact(RequiredTag);
	}

#if WITH_EDITOR
	virtual FString GetEditorSummary() const override
	{
		return FString::Printf(TEXT("Context has tag %s"), *RequiredTag.ToString());
	}
#endif
};

