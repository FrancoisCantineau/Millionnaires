#include "UI/QuestJournalWidget.h"
#include "QuestComponent.h"

void UQuestJournalWidget::SetQuestComponent(UQuestComponent* InQuestComponent)
{
	if (QuestComponent)
	{
		QuestComponent->OnQuestStateChanged.RemoveDynamic(this, &UQuestJournalWidget::HandleQuestStateChanged);
	}

	QuestComponent = InQuestComponent;

	if (QuestComponent)
	{
		QuestComponent->OnQuestStateChanged.AddDynamic(this, &UQuestJournalWidget::HandleQuestStateChanged);
	}

	RefreshEntries();
}

void UQuestJournalWidget::HandleQuestStateChanged(FName QuestId)
{
	RefreshEntries();
}

void UQuestJournalWidget::RefreshEntries()
{
	ActiveEntries.Empty();
	FinishedEntries.Empty();

	if (!QuestComponent)
	{
		OnJournalUpdated();
		return;
	}

	for (const FQuestRuntimeState& RuntimeState : QuestComponent->GetActiveQuestStates())
	{
		UQuestDefinition* Definition = QuestComponent->GetQuestDefinition(RuntimeState.QuestId);
		if (!Definition)
		{
			// Runtime state exists but the definition isn't registered on this machine (e.g. a client
			// that never called RegisterQuestDefinition for it) - nothing sensible to display, skip it
			// rather than showing a blank entry.
			continue;
		}

		FQuestJournalEntry Entry;
		Entry.QuestId = RuntimeState.QuestId;
		Entry.Title = Definition->Title;
		Entry.Description = Definition->Description;
		Entry.QuestType = Definition->QuestType;
		Entry.State = RuntimeState.State;

		for (const FQuestObjectiveRuntimeState& ObjRuntime : RuntimeState.Objectives)
		{
			const FQuestObjectiveEntry* ObjDef = Definition->Objectives.FindByPredicate(
				[&ObjRuntime](const FQuestObjectiveEntry& E) { return E.ObjectiveId == ObjRuntime.ObjectiveId; });

			FQuestJournalObjectiveEntry ObjEntry;
			ObjEntry.ObjectiveId = ObjRuntime.ObjectiveId;
			ObjEntry.Description = (ObjDef && ObjDef->ObjectiveTemplate) ? ObjDef->ObjectiveTemplate->GetObjectiveDescription() : FText::GetEmpty();
			ObjEntry.State = ObjRuntime.State;
			ObjEntry.bHasProgress = ObjRuntime.ProgressTarget >= 0;
			ObjEntry.ProgressCurrent = ObjRuntime.ProgressCurrent;
			ObjEntry.ProgressTarget = ObjRuntime.ProgressTarget;
			ObjEntry.DisplayFields = ObjRuntime.DisplayFields;
			ObjEntry.bHasTimeLimit = ObjRuntime.ActivationServerTime >= 0.0f;
			ObjEntry.ActivationServerTime = ObjRuntime.ActivationServerTime;
			ObjEntry.TimeLimitSeconds = (ObjDef && ObjDef->ObjectiveTemplate) ? ObjDef->ObjectiveTemplate->GetTimeLimitSeconds() : 0.0f;

			Entry.Objectives.Add(ObjEntry);
		}

		// Active -> ActiveEntries. Completed or Failed -> FinishedEntries (both kept - Failed is
		// never dropped, only routed differently; Blueprint branches on Entry.State to style
		// Failed distinctly, e.g. struck-through in red, per François's design choice).
		if (Entry.State == EQuestState::Active)
		{
			ActiveEntries.Add(Entry);
		}
		else
		{
			FinishedEntries.Add(Entry);
		}
	}

	OnJournalUpdated();
}
