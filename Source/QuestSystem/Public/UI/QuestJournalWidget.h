#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestJournalEntry.h"
#include "QuestJournalWidget.generated.h"

class UQuestComponent;

/**
 * Base class for the quest journal UI. Does the data assembly (merging static UQuestDefinition
 * content with the replicated FQuestRuntimeState progress into display-ready FQuestJournalEntry
 * structs) so a Blueprint child only has to loop through GetActiveEntries()/GetFinishedEntries() and bind widgets -
 * same split as UQuestObjectiveEntryWidget already uses.
 *
 * Works identically on server or client: pulls from UQuestComponent::GetActiveQuestStates(),
 * which is a plain replicated snapshot either way - the widget doesn't know or care whether
 * it's running on the listen server or a remote client.
 */
UCLASS(Abstract, Blueprintable)
class QUESTSYSTEM_API UQuestJournalWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Call once, e.g. right after creating the widget: GetOwningPlayerState -> QuestComponent -> this. */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void SetQuestComponent(UQuestComponent* InQuestComponent);

	/** Rebuilds ActiveEntries/FinishedEntries from the current state. Called automatically whenever the bound
	 *  QuestComponent's OnQuestStateChanged fires, but callable manually too (e.g. right after SetQuestComponent). */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void RefreshEntries();

	UFUNCTION(BlueprintPure, Category = "Quest")
	const TArray<FQuestJournalEntry>& GetActiveEntries() const { return ActiveEntries; }

	/** Completed AND Failed quests - branch on Entry.State in Blueprint to style them
	 *  differently (e.g. Failed stays struck-through in red, Completed gets a checkmark). */
	UFUNCTION(BlueprintPure, Category = "Quest")
	const TArray<FQuestJournalEntry>& GetFinishedEntries() const { return FinishedEntries; }

	/** Override in Blueprint to rebuild the visible lists whenever ActiveEntries/FinishedEntries changes. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Quest")
	void OnJournalUpdated();

protected:

	UFUNCTION()
	void HandleQuestStateChanged(FName QuestId);

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	TArray<FQuestJournalEntry> ActiveEntries;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	TArray<FQuestJournalEntry> FinishedEntries;

	UPROPERTY()
	TObjectPtr<UQuestComponent> QuestComponent;
};
