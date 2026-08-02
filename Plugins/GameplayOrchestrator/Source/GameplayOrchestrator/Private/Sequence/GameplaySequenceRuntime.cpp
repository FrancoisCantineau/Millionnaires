#include "Sequence/GameplaySequenceRuntime.h"
#include "Sequence/GameplaySequence.h"
#include "Actions/GameplayActionRuntime.h"

void UGameplaySequenceRuntime::Start(UGameplaySequence* InSequence, const FEventContext& InContext, UObject* InWorldContextObject)
{
	Sequence = InSequence;
	Context = InContext;
	WorldContextObject = InWorldContextObject;
	State = ESequenceRuntimeState::Running;
	CurrentStepIndex = -1;

	ExecuteStep(0);
}

void UGameplaySequenceRuntime::ExecuteStep(int32 Index)
{
	if (State != ESequenceRuntimeState::Running)
	{
		return;
	}

	CurrentStepIndex = Index;

	if (!Sequence || !Sequence->Steps.IsValidIndex(Index))
	{
		// Plus de Step : la séquence est arrivée à son terme naturellement.
		FinishAs(ESequenceRuntimeState::Finished);
		return;
	}

	const FGameplaySequenceStep& Step = Sequence->Steps[Index];

	// On compte les actions valides AVANT de lancer quoi que ce soit, pour
	// que CheckStepCompletion() ne se déclenche pas prématurément si une
	// action Instant termine (et décrémente) avant que les autres actions
	// du même Step aient même été lancées.
	PendingActionsInCurrentStep = 0;
	for (const TObjectPtr<UGameplayActionBase>& Action : Step.ParallelActions)
	{
		if (Action)
		{
			++PendingActionsInCurrentStep;
		}
	}

	if (PendingActionsInCurrentStep == 0)
	{
		// Step vide (ou uniquement des entrées nulles) : on passe directement au suivant.
		ExecuteStep(Index + 1);
		return;
	}

	// Copie défensive : une action Instant pourrait, en théorie, déclencher
	// une ré-entrance qui modifie Step.ParallelActions indirectement (peu
	// probable ici puisque Sequence est un asset immutable à l'exécution,
	// mais on reste prudent).
	TArray<TObjectPtr<UGameplayActionBase>> ActionsToLaunch = Step.ParallelActions;

	for (const TObjectPtr<UGameplayActionBase>& Action : ActionsToLaunch)
	{
		if (!Action)
		{
			continue;
		}

		if (State != ESequenceRuntimeState::Running)
		{
			// Une action précédente du même Step a pu déclencher un Cancel
			// de manière synchrone (cas limite) - on arrête d'en lancer d'autres.
			return;
		}

		const EGameplayActionExecutionType ExecutionType = Action->GetExecutionType();

		if (ExecutionType == EGameplayActionExecutionType::Instant)
		{
			Action->Execute(Context);
			// Une action Instant est considérée terminée dès son retour.
			--PendingActionsInCurrentStep;
		}
		else
		{
			UGameplayActionRuntime* ActionRuntime = NewObject<UGameplayActionRuntime>(this);
			ActionRuntime->Action = Action;
			ActionRuntime->OwningSequenceRuntime = this;
			ActionRuntime->WorldContextObject = WorldContextObject;
			ActiveActionRuntimes.Add(ActionRuntime);

			Action->ExecuteLatent(ActionRuntime, Context);
			// L'action Latent décrémentera via OnActionRuntimeFinished quand
			// elle appellera ActionRuntime->NotifyFinished(...).
		}
	}

	CheckStepCompletion();
}

void UGameplaySequenceRuntime::OnActionRuntimeFinished(UGameplayActionRuntime* ActionRuntime, EGameplayActionResult Result)
{
	if (State != ESequenceRuntimeState::Running)
	{
		// La séquence a déjà été annulée/terminée entre-temps (ex: notification
		// tardive après un Cancel()) - on ignore silencieusement.
		return;
	}

	ActiveActionRuntimes.RemoveSingleSwap(ActionRuntime);

	// NB : Failed n'entraîne PAS d'arrêt de la séquence pour l'instant - le
	// Runtime ne fait pas encore de branchement conditionnel sur le résultat
	// (voir commentaire sur EGameplayActionResult). Failed est traité comme
	// Succeeded du point de vue de l'avancement du Step.
	--PendingActionsInCurrentStep;
	CheckStepCompletion();
}

void UGameplaySequenceRuntime::CheckStepCompletion()
{
	if (State != ESequenceRuntimeState::Running)
	{
		return;
	}

	if (PendingActionsInCurrentStep <= 0)
	{
		ExecuteStep(CurrentStepIndex + 1);
	}
}

void UGameplaySequenceRuntime::Cancel()
{
	if (State != ESequenceRuntimeState::Running)
	{
		return;
	}

	State = ESequenceRuntimeState::Cancelled;

	// Copie défensive : ActionRuntime::Cancel() pourrait modifier la liste
	// (bien qu'il ne le fasse pas dans l'implémentation actuelle).
	TArray<TObjectPtr<UGameplayActionRuntime>> RuntimesToCancel = ActiveActionRuntimes;
	for (const TObjectPtr<UGameplayActionRuntime>& ActionRuntime : RuntimesToCancel)
	{
		if (ActionRuntime)
		{
			ActionRuntime->Cancel();
		}
	}
	ActiveActionRuntimes.Reset();

	FinishAs(ESequenceRuntimeState::Cancelled);
}

void UGameplaySequenceRuntime::FinishAs(ESequenceRuntimeState FinalState)
{
	State = FinalState;
	OnFinished.Broadcast(this);
}
