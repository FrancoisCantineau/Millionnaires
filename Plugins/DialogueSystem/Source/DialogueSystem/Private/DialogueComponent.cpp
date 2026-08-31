// DialogueComponent.cpp
#include "DialogueComponent.h"
#include "DialogueDataAsset.h"
#include "Sound/SoundBase.h"

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
	OnLineStarted(Node->Line);

	if (Node->Choices.Num() > 0)
	{
		OnChoicesPresented(Node->Choices);
		return;
	}

	// Linear node — auto-advance once the line's voice line finishes (or FallbackDuration if none).
	const float Duration = Node->Line.VoiceLine ? Node->Line.VoiceLine->GetDuration() : Node->Line.FallbackDuration;
	GetWorld()->GetTimerManager().SetTimer(AutoAdvanceTimerHandle, this, &UDialogueComponent::AutoAdvance, FMath::Max(Duration, 0.1f), false);
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

void UDialogueComponent::SelectChoice(int32 ChoiceIndex)
{
	if (!CurrentDialogue)
	{
		return;
	}

	const FDialogueNode* Node = CurrentDialogue->FindNode(CurrentNodeID);
	if (!Node || !Node->Choices.IsValidIndex(ChoiceIndex))
	{
		return;
	}

	PlayNode(Node->Choices[ChoiceIndex].NextNodeID);
}

void UDialogueComponent::EndDialogue()
{
	GetWorld()->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
	CurrentDialogue = nullptr;
	CurrentNodeID = NAME_None;
	SpeakingActor = nullptr;
	OnDialogueEnded();
}
