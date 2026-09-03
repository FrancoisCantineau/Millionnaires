// DialogueComponent.cpp
#include "DialogueComponent.h"
#include "DialogueDataAsset.h"
#include "DialogueWidgetBase.h"
#include "Sound/SoundBase.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UDialogueComponent::UDialogueComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDialogueComponent::StartDialogue(UDialogueDataAsset* Dialogue, AActor* InSpeakingActor)
{
	if (!Dialogue || Dialogue->Nodes.Num() == 0)
	{
		return;
	}

	if (CurrentDialogue)
	{
		EndDialogue();
	}

	CurrentDialogue = Dialogue;
	SpeakingActor = InSpeakingActor;
	SeenDialogueIDs.Add(Dialogue->GetPrimaryAssetId());

	if (WidgetClass && !ActiveWidget)
	{
		APlayerController* PC = Cast<APlayerController>(GetOwner());
		if (!PC)
		{
			PC = UGameplayStatics::GetPlayerController(this, 0);
		}
		if (PC)
		{
			ActiveWidget = CreateWidget<UDialogueWidgetBase>(PC, WidgetClass);
			if (ActiveWidget)
			{
				ActiveWidget->OwningDialogueComponent = this;
				ActiveWidget->AddToViewport();
			}
		}
	}

	PlayNode(Dialogue->StartNodeID);
}

void UDialogueComponent::PlayNode(FName NodeID)
{
	if (!CurrentDialogue)
	{
		return;
	}

	const FDialogueNode* Node = CurrentDialogue->FindNode(NodeID);
	if (!Node)
	{
		// NodeID not found (or NAME_None reached) — dialogue is over.
		EndDialogue();
		return;
	}

	CurrentNodeID = NodeID;
	bAwaitingExternalLineFinish = false;

	FDialoguePlayedLine Entry;
	Entry.Line = Node->Line;
	Entry.WorldTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	History.Add(Entry);
	if (History.Num() > MaxHistoryEntries)
	{
		History.RemoveAt(0, History.Num() - MaxHistoryEntries);
	}

	OnLineStarted(Node->Line);
	if (ActiveWidget)
	{
		ActiveWidget->OnLineStarted(Node->Line);
	}

	if (Node->EventTag.IsValid())
	{
		OnNodeEventTriggered(Node->EventTag);
	}

	if (Node->Choices.Num() > 0)
	{
		PresentedChoices.Reset();
		for (const FDialogueChoice& Choice : Node->Choices)
		{
			if (Choice.RequiredTags.IsEmpty() || Choice.RequiredTags.Matches(CurrentContextTags))
			{
				PresentedChoices.Add(Choice);
			}
		}

		if (PresentedChoices.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("[DialogueComponent] Node '%s' has choices but none satisfy CurrentContextTags — ending dialogue."), *NodeID.ToString());
			EndDialogue();
			return;
		}

		bWaitingOnChoice = true;
		OnChoicesPresented(PresentedChoices);
		if (ActiveWidget)
		{
			ActiveWidget->OnChoicesPresented(PresentedChoices);
		}
		return;
	}

	// Linear node — advance once the line finishes. If there's a VoiceLine, prefer an external
	// NotifyLineFinished() call for exact sync; GetDuration() (+margin) is only a safety fallback
	// in case that's never called, so a dialogue can never get stuck waiting forever.
	bWaitingOnChoice = false;
	float TimerDuration;
	if (Node->Line.VoiceLine)
	{
		bAwaitingExternalLineFinish = true;
		TimerDuration = Node->Line.VoiceLine->GetDuration() + 1.0f;
	}
	else
	{
		TimerDuration = Node->Line.FallbackDuration;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(AutoAdvanceTimerHandle, this, &UDialogueComponent::AutoAdvance, FMath::Max(TimerDuration, 0.1f), false);
	}
}

void UDialogueComponent::NotifyLineFinished()
{
	if (!CurrentDialogue || bWaitingOnChoice || !bAwaitingExternalLineFinish)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
	}
	AutoAdvance();
}

void UDialogueComponent::AutoAdvance()
{
	if (!CurrentDialogue)
	{
		return;
	}

	const FDialogueNode* Node = CurrentDialogue->FindNode(CurrentNodeID);
	const FName NextID = Node ? Node->NextNodeID : NAME_None;
	PlayNode(NextID);
}

void UDialogueComponent::SkipCurrentLine()
{
	if (!CurrentDialogue || bWaitingOnChoice)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World && World->GetTimerManager().IsTimerActive(AutoAdvanceTimerHandle))
	{
		World->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
		AutoAdvance();
	}
}

void UDialogueComponent::SelectChoice(int32 ChoiceIndex)
{
	if (!CurrentDialogue || !bWaitingOnChoice)
	{
		return;
	}

	if (!PresentedChoices.IsValidIndex(ChoiceIndex))
	{
		return;
	}

	const FDialogueChoice Choice = PresentedChoices[ChoiceIndex];
	bWaitingOnChoice = false;

	if (!Choice.TagsToGrantOnSelect.IsEmpty())
	{
		OnChoiceTagsGranted(Choice.TagsToGrantOnSelect);
	}

	PlayNode(Choice.NextNodeID);
}

bool UDialogueComponent::HasSeenDialogue(const UDialogueDataAsset* Dialogue) const
{
	return Dialogue && SeenDialogueIDs.Contains(Dialogue->GetPrimaryAssetId());
}

FDialogueSaveData UDialogueComponent::CaptureSaveData() const
{
	FDialogueSaveData SaveData;
	SaveData.SeenDialogues = SeenDialogueIDs.Array();
	SaveData.History = History;
	return SaveData;
}

void UDialogueComponent::RestoreSaveData(const FDialogueSaveData& SaveData)
{
	SeenDialogueIDs.Reset();
	for (const FPrimaryAssetId& ID : SaveData.SeenDialogues)
	{
		SeenDialogueIDs.Add(ID);
	}
	History = SaveData.History;
}

void UDialogueComponent::EndDialogue()
{
	UDialogueDataAsset* EndedDialogue = CurrentDialogue;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
	}

	CurrentDialogue = nullptr;
	CurrentNodeID = NAME_None;
	SpeakingActor = nullptr;
	bWaitingOnChoice = false;
	bAwaitingExternalLineFinish = false;
	PresentedChoices.Reset();

	OnDialogueEnded(EndedDialogue);
	OnDialogueEndedDelegate.Broadcast(EndedDialogue);

	if (ActiveWidget)
	{
		ActiveWidget->OnDialogueEnded();
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}
}
