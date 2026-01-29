#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "QuestTrackerWidget.generated.h"

class UQuestObjective;
class UVerticalBox;
class UTextBlock;
class UQuestObjectiveEntryWidget;

/**
 * Widget to track and display active quest objectives
 * - Shows list of current objectives with progress
 * - Updates dynamically as objectives are completed or updated
 */
UCLASS()
class QUESTSYSTEM_API UQuestTrackerWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Initialize widget with quest manager */
	void InitializeTracker(class UQuestManagerComponent* InQuestManager);

	/** Add or update objective in tracker */
	UFUNCTION()
	void UpdateObjective(UQuestObjective* Objective);

	/** Remove completed objective from list */
	UFUNCTION()
	void RemoveObjective(UQuestObjective* Objective);

	/** Clear all objectives */
	void ClearAllObjectives();

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Title text */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ObjectiveTitleText;

	/** Container for objective entries */
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* ObjectiveListContainer;

	/** Widget class for individual objective entries */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UQuestObjectiveEntryWidget> ObjectiveEntryClass;

	/** Reference to quest manager */
	UPROPERTY()
	UQuestManagerComponent* QuestManager;

	/** Current objective entry widgets */
	UPROPERTY()
	TMap<UQuestObjective*, UQuestObjectiveEntryWidget*> ObjectiveEntries;

private:

	/** Create entry for objective */
	UQuestObjectiveEntryWidget* CreateObjectiveEntry(UQuestObjective* Objective);
	
	/** Cleanup delegates */
	void CleanupDelegates();
};