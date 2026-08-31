#include "Bridges/OrchestratorBridges/Dialogue/GameplayAction_StartDialogue.h"
#include "DialogueComponent.h"
#include "DialogueDataAsset.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Core/GameplayEventContext.h"

void UGameplayAction_StartDialogue::Execute_Implementation(const FEventContext& Context)
{
	if (!DialogueData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameplayAction_StartDialogue] DialogueData not set, nothing started."));
		return;
	}

	APawn* Pawn = Cast<APawn>(Context.Instigator.Get());
	APlayerController* PC = Pawn ? Cast<APlayerController>(Pawn->GetController()) : Cast<APlayerController>(Context.Instigator.Get());
	UDialogueComponent* DialogueComp = PC ? PC->FindComponentByClass<UDialogueComponent>() : nullptr;

	if (!DialogueComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameplayAction_StartDialogue] No UDialogueComponent found on Instigator's PlayerController."));
		return;
	}

	// TODO: SpeakingActor (the NPC) isn't available from Context.Instigator (that's the player).
	// Passing nullptr for now — wire up whatever your Orchestrator exposes for "the NPC/sequence
	// owner" here once confirmed, so bBroadcastIfPlayerFar wiring downstream has something to
	// measure distance from.
	DialogueComp->StartDialogue(DialogueData, nullptr);
}

#if WITH_EDITOR
FString UGameplayAction_StartDialogue::GetEditorSummary() const
{
	return DialogueData ? FString::Printf(TEXT("Start Dialogue: %s"), *DialogueData->GetName()) : TEXT("Start Dialogue: (none)");
}
#endif