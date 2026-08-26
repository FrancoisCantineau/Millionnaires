#pragma once

#include "CoreMinimal.h"
#include "GameplayActionBase.h"
#include "GameplayTagContainer.h"
#include "GameplayAction_BroadcastEvent.generated.h"

/**
 * Publishes a tag (with optional extra tags and an optional object payload) on the
 * GameplayEventBus, and ends instantly - the sequence doesn't wait for whoever, if anyone,
 * reacts to it. Mirror of Wait Gameplay Event, but on the emitting side.
 *
 * Generic on purpose: never references Quest/Inventory/Ship or any other domain plugin.
 * PayloadObject is a plain UObject* so this action never needs a header from whichever
 * plugin defines the concrete payload type (e.g. a UQuestDefinition or an alert DataAsset) -
 * the listener on the other end is the one that knows what to Cast<> it to.
 */
UCLASS(meta = (DisplayName = "Broadcast Event"))
class GAMEPLAYORCHESTRATOR_API UGameplayAction_BroadcastEvent : public UGameplayActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Broadcast Event")
	FGameplayTag EventTag;

	/** Extra routing/data tags for the listener - e.g. a quest ID tag, an alert zone tag. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Broadcast Event")
	FGameplayTagContainer AdditionalTags;

	/** Optional object payload - a quest/item/alert DataAsset, whatever the listener's tag
	 *  contract expects. No AllowedClasses filter here on purpose: this action is meant to stay
	 *  usable for any domain. Prefer a dedicated typed action (e.g. "Trigger Quest") over this
	 *  one when a payload type is used often enough to be worth a filtered picker. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Broadcast Event")
	TObjectPtr<UObject> PayloadObject;

	virtual void Execute_Implementation(const FEventContext& Context) override;

#if WITH_EDITOR
	virtual FString GetEditorSummary() const override
	{
		return FString::Printf(TEXT("Broadcast %s"), *EventTag.ToString());
	}
#endif
};
