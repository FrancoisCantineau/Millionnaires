#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestJournalEntry.h"
#include "QuestJournalObjectiveEntryWidget.generated.h"

class UTextBlock;
class UProgressBar;

/**
 * One row for one objective inside a quest. Same struct-in, BindWidget-out pattern as
 * UQuestJournalEntryWidget. ProgressText/ProgressBar only get filled in when the objective
 * actually has numeric progress (bHasProgress) - e.g. a plain "go here" objective just
 * shows its description, no "0/0" showing up.
 *
 * The timer (TimerText) is NOT part of the replicated snapshot as live text - only a one-shot
 * ActivationServerTime + TimeLimitSeconds are. This widget computes the countdown itself every
 * frame via NativeTick, purely locally - no network chatter needed for a smooth countdown.
 */
UCLASS(Blueprintable)
class QUESTSYSTEM_API UQuestJournalObjectiveEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void SetEntry(const FQuestJournalObjectiveEntry& InEntry);

	UFUNCTION(BlueprintPure, Category = "Quest")
	const FQuestJournalObjectiveEntry& GetEntry() const { return Entry; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest")
	void OnEntrySet();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DescriptionText;

	/** Only set when Entry.bHasProgress is true - e.g. "3/5". */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ProgressText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> ProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TimerText;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FQuestJournalObjectiveEntry Entry;
};
