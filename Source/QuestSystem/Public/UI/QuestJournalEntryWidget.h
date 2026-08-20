#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestJournalEntry.h"
#include "QuestJournalEntryWidget.generated.h"

class UTextBlock;

/**
 * One row in the journal - one quest. Create a WBP child of this, bind the text widgets
 * (they're optional, so nothing crashes if you only wire up some of them), place it inside
 * WBP_QuestJournal's OnJournalUpdated loop.
 */
UCLASS(Blueprintable)
class QUESTSYSTEM_API UQuestJournalEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void SetEntry(const FQuestJournalEntry& InEntry);

	UFUNCTION(BlueprintPure, Category = "Quest")
	const FQuestJournalEntry& GetEntry() const { return Entry; }

	/** Fires after SetEntry populates the bound text widgets, in case you want extra Blueprint logic
	 *  (e.g. spawn one UQuestJournalObjectiveEntryWidget per objective into a container). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Quest")
	void OnEntrySet();

protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DescriptionText;

	/** e.g. "Main Quest" / "Side Quest" - text form of QuestType, for convenience. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> QuestTypeText;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FQuestJournalEntry Entry;
};
