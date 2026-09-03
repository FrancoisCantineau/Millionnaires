#include "Bridges/OrchestratorBridges/Dialogue/GameplayAction_StartDialogue.h"
#include "DialogueComponent.h"
#include "DialogueDataAsset.h"
#include "Actions/GameplayActionRuntime.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Core/GameplayEventContext.h"

EGameplayActionExecutionType UGameplayAction_StartDialogue::GetExecutionType_Implementation()
{
	return EGameplayActionExecutionType::Latent;
}

void UGameplayAction_StartDialogue::ExecuteLatent_Implementation(UGameplayActionRuntime* ActionRuntime, const FEventContext& Context)
{
	if (!DialogueData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameplayAction_StartDialogue] DialogueData not set, nothing started."));
		if (ActionRuntime)
		{
			ActionRuntime->NotifyFinished(EGameplayActionResult::Failed);
		}
		return;
	}

	APawn* Pawn = Cast<APawn>(Context.Instigator.Get());
	APlayerController* PC = Pawn ? Cast<APlayerController>(Pawn->GetController()) : Cast<APlayerController>(Context.Instigator.Get());
	UDialogueComponent* DialogueComp = PC ? PC->FindComponentByClass<UDialogueComponent>() : nullptr;

	if (!DialogueComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameplayAction_StartDialogue] No UDialogueComponent found on Instigator's PlayerController."));
		if (ActionRuntime)
		{
			ActionRuntime->NotifyFinished(EGameplayActionResult::Failed);
		}
		return;
	}

	BoundDialogueComp = DialogueComp;
	CachedRuntime = ActionRuntime;
	DialogueComp->OnDialogueEndedDelegate.AddDynamic(this, &UGameplayAction_StartDialogue::HandleDialogueEnded);

	// Sender is the Actor that raised this event (the NPC, per FEventContext::Make's convention),
	// while Instigator is whoever's responsible for it (the player).
	DialogueComp->StartDialogue(DialogueData, Context.Sender.Get());
}

void UGameplayAction_StartDialogue::HandleDialogueEnded(UDialogueDataAsset* EndedDialogue)
{
	if (BoundDialogueComp)
	{
		BoundDialogueComp->OnDialogueEndedDelegate.RemoveDynamic(this, &UGameplayAction_StartDialogue::HandleDialogueEnded);
	}

	if (CachedRuntime)
	{
		CachedRuntime->NotifyFinished(EGameplayActionResult::Succeeded);
	}
}

void UGameplayAction_StartDialogue::OnCancelLatent(UGameplayActionRuntime* ActionRuntime)
{
	// Generic cleanup (timer/bus subscription) is already handled by ActionRuntime per the base
	// class contract — this only needs our OWN specific cleanup: unsubscribe and stop the
	// dialogue itself. Do NOT call NotifyFinished here.
	if (BoundDialogueComp)
	{
		BoundDialogueComp->OnDialogueEndedDelegate.RemoveDynamic(this, &UGameplayAction_StartDialogue::HandleDialogueEnded);
		BoundDialogueComp->EndDialogue();
	}
}

#if WITH_EDITOR
FString UGameplayAction_StartDialogue::GetEditorSummary() const
{
	return DialogueData ? FString::Printf(TEXT("Start Dialogue: %s"), *DialogueData->GetName()) : TEXT("Start Dialogue: (none)");
}
#endif