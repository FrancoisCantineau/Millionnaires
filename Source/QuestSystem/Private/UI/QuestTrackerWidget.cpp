#include "UI/QuestTrackerWidget.h"
#include "UI/QuestObjectiveEntryWidget.h"
#include "QuestObjective.h"
#include "QuestManagerComponent.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

void UQuestTrackerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ObjectiveTitleText)
	{
		ObjectiveTitleText->SetText(FText::FromString("OBJECTIVES"));
	}
}

void UQuestTrackerWidget::NativeDestruct()
{
	CleanupDelegates();
	Super::NativeDestruct();
}

void UQuestTrackerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	TArray<UQuestObjective*> ObjectivesToUpdate;
	ObjectiveEntries.GetKeys(ObjectivesToUpdate);

	for (UQuestObjective* Obj : ObjectivesToUpdate)
	{
		if (IsValid(Obj))
		{
			UQuestObjectiveEntryWidget** EntryPtr = ObjectiveEntries.Find(Obj);
			if (EntryPtr && IsValid(*EntryPtr))
			{
				(*EntryPtr)->UpdateProgress();
			}
		}
	}
}

/*
 * Initialize the quest tracker with the given quest manager
 */
void UQuestTrackerWidget::InitializeTracker(UQuestManagerComponent* InQuestManager)
{
	CleanupDelegates();

	QuestManager = InQuestManager;

	if (IsValid(QuestManager))
	{
		QuestManager->OnObjectiveStartedEvent.AddDynamic(this, &UQuestTrackerWidget::UpdateObjective);
		QuestManager->OnObjectiveCompletedEvent.AddDynamic(this, &UQuestTrackerWidget::RemoveObjective);
		QuestManager->OnObjectiveFailedEvent.AddDynamic(this, &UQuestTrackerWidget::RemoveObjective);

		UQuestObjective* CurrentObjective = QuestManager->GetCurrentObjective();
		if (IsValid(CurrentObjective))
		{
			UpdateObjective(CurrentObjective);
		}
	}
}

/*
 * Add or update an objective in the tracker
 */
void UQuestTrackerWidget::UpdateObjective(UQuestObjective* Objective)
{
	
	if (!IsValid(Objective))
	{
		UE_LOG(LogTemp, Error, TEXT("UQuestTrackerWidget::UpdateObjective called with invalid Objective"));
		return;
	}
	
	if (!IsValid(ObjectiveListContainer))
	{
		UE_LOG(LogTemp, Error, TEXT("UQuestTrackerWidget::UpdateObjective: Invalid ObjectiveListContainer"));
		return;
	}

	if (ObjectiveEntries.Contains(Objective))
	{
		UE_LOG(LogTemp, Warning, TEXT("UQuestTrackerWidget::UpdateObjective: Objective already tracked"));
		return;
	}

	UQuestObjectiveEntryWidget* NewEntry = CreateObjectiveEntry(Objective);
	if (IsValid(NewEntry))
	{
		ObjectiveEntries.Add(Objective, NewEntry);
		ObjectiveListContainer->AddChild(NewEntry);
	}
}

void UQuestTrackerWidget::RemoveObjective(UQuestObjective* Objective)
{
	if (!IsValid(Objective) || !IsValid(ObjectiveListContainer))
	{
		UE_LOG(LogTemp, Warning, TEXT("UQuestTrackerWidget::RemoveObjective: Invalid parameters"));
		return;
	}

	UQuestObjectiveEntryWidget** EntryPtr = ObjectiveEntries.Find(Objective);
	if (EntryPtr && IsValid(*EntryPtr))
	{
		UQuestObjectiveEntryWidget* Entry = *EntryPtr;
		
		Entry->PlayCompletionAnimation();
		
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindWeakLambda(this, [this, Objective, Entry]()
		{
			if (IsValid(this) && IsValid(ObjectiveListContainer) && IsValid(Entry))
			{
				ObjectiveListContainer->RemoveChild(Entry);
				ObjectiveEntries.Remove(Objective);
			}
		});

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.0f, false);
		}
	}
}

/*
 * Clear all objectives from the tracker
 */
void UQuestTrackerWidget::ClearAllObjectives()
{
	if (IsValid(ObjectiveListContainer))
	{
		ObjectiveListContainer->ClearChildren();
	}
	ObjectiveEntries.Empty();
}

/*
 * Create a new objective entry widget for the given objective
 */
UQuestObjectiveEntryWidget* UQuestTrackerWidget::CreateObjectiveEntry(UQuestObjective* Objective)
{
	if (!ObjectiveEntryClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UQuestTrackerWidget::CreateObjectiveEntry: ObjectiveEntryClass not set"));
		return nullptr;
	}

	UQuestObjectiveEntryWidget* NewEntry = CreateWidget<UQuestObjectiveEntryWidget>(this, ObjectiveEntryClass);
	if (IsValid(NewEntry))
	{
		NewEntry->InitializeObjective(Objective);
	}

	return NewEntry;
}

/*
 * Cleanup delegate bindings
 */
void UQuestTrackerWidget::CleanupDelegates()
{
	if (IsValid(QuestManager))
	{
		QuestManager->OnObjectiveStartedEvent.RemoveDynamic(this, &UQuestTrackerWidget::UpdateObjective);
		QuestManager->OnObjectiveCompletedEvent.RemoveDynamic(this, &UQuestTrackerWidget::RemoveObjective);
		QuestManager->OnObjectiveFailedEvent.RemoveDynamic(this, &UQuestTrackerWidget::RemoveObjective);
	}
}