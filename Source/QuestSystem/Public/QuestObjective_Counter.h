#pragma once

#include "CoreMinimal.h"
#include "QuestObjective.h"
#include "GameplayTagContainer.h"
#include "QuestObjective_Counter.generated.h"

/**
 * Generic "do N of X" objective (repair equipment, kill enemies, collect items...).
 *
 * Fully decoupled from whatever triggers the counting: it just listens for
 * ListenEventTag on the GameplayEventBus and increments on each occurrence.
 * The source of that event (a repair actor, an enemy death, a pickup) never
 * needs to know a quest exists.
 */
UCLASS(Blueprintable)
class QUESTSYSTEM_API UQuestObjective_Counter : public UQuestObjective
{
	GENERATED_BODY()

public:

	UQuestObjective_Counter();

	virtual void Initialize(UObject* InOwner) override;
	virtual void ObjectiveActivate() override;
	virtual void ObjectiveComplete() override;
	virtual void ObjectiveFail() override;

	UFUNCTION(BlueprintPure, Category = "Quest")
	int32 GetCurrentCount() const { return CurrentCount; }

	UFUNCTION(BlueprintPure, Category = "Quest")
	int32 GetTargetCount() const { return TargetCount; }

	UFUNCTION(BlueprintPure, Category = "Quest")
	float GetProgress() const { return TargetCount > 0 ? FMath::Clamp((float)CurrentCount / (float)TargetCount, 0.0f, 1.0f) : 0.0f; }

	virtual int32 GetProgressCurrent() const override { return CurrentCount; }
	virtual int32 GetProgressTarget() const override { return TargetCount; }

protected:

	/** Tag this objective listens for on the GameplayEventBus. Each occurrence = +1. */
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	FGameplayTag ListenEventTag;

	/** How many occurrences are needed to complete the objective. */
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	int32 TargetCount = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	int32 CurrentCount = 0;

private:

	void HandleEvent(const struct FEventContext& Context);
	void UnsubscribeFromBus();

	FDelegateHandle BusSubscriptionHandle;
};
