#include "Component/GameplayOrchestratorComponent.h"
#include "Event/GameplayEventBus.h"
#include "State/GameplayWorldStateSubsystem.h"
#include "Sequence/GameplaySequence.h"
#include "Sequence/GameplaySequenceRuntime.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

UGameplayOrchestratorComponent::UGameplayOrchestratorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGameplayOrchestratorComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (StateIdentityTag.IsValid())
	{
		if (UGameInstance* GameInstance = Owner->GetGameInstance())
		{
			if (UGameplayWorldStateSubsystem* StateSubsystem = GameInstance->GetSubsystem<UGameplayWorldStateSubsystem>())
			{
				StateSubsystem->RegisterActor(StateIdentityTag, Owner);
			}
		}
	}
}

void UGameplayOrchestratorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AActor* Owner = GetOwner();
	if (Owner && StateIdentityTag.IsValid())
	{
		if (UGameInstance* GameInstance = Owner->GetGameInstance())
		{
			if (UGameplayWorldStateSubsystem* StateSubsystem = GameInstance->GetSubsystem<UGameplayWorldStateSubsystem>())
			{
				StateSubsystem->UnregisterActor(StateIdentityTag, Owner);
			}
		}
	}

	// Indispensable : si cet Actor est détruit pendant qu'une Sequence locale
	// est en attente (ex: Delay en cours), on annule proprement plutôt que de
	// laisser un timer pointer vers un Runtime dont le contexte (Owner) va
	// disparaître.
	TArray<TObjectPtr<UGameplaySequenceRuntime>> RuntimesToCancel = LocalActiveRuntimes;
	for (const TObjectPtr<UGameplaySequenceRuntime>& Runtime : RuntimesToCancel)
	{
		if (Runtime)
		{
			Runtime->Cancel();
		}
	}
	LocalActiveRuntimes.Reset();
	LocalRunningByDefinition.Reset();
	LocalQueuedContexts.Reset();

	Super::EndPlay(EndPlayReason);
}

void UGameplayOrchestratorComponent::EmitEvent(FGameplayTag EventTag, AActor* Instigator)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	FEventContext Context = FEventContext::Make(EventTag, Owner, Instigator ? Instigator : Owner);

	// 1) Évaluation LOCALE directe : coût proportionnel uniquement au nombre
	// de LocalEvents de CET Actor, jamais au nombre d'Actors partageant le
	// même tag ailleurs dans le niveau.
	for (const TObjectPtr<UGameplayEventDefinition>& Definition : LocalEvents)
	{
		if (!Definition || Definition->EventTag != EventTag)
		{
			continue;
		}
		if (Definition->bOneShot && LocalFiredOneShot.Contains(Definition))
		{
			continue;
		}
		if (!Definition->EvaluateConditions(Context))
		{
			continue;
		}
		if (Definition->bOneShot)
		{
			LocalFiredOneShot.Add(Definition);
		}

		// ExecutionPolicy, appliquée localement de la même façon que dans
		// GameplayEventSubsystem (voir ce fichier pour le détail du raisonnement).
		TArray<TObjectPtr<UGameplaySequenceRuntime>>& RunningList = LocalRunningByDefinition.FindOrAdd(Definition);
		RunningList.RemoveAll([](const TObjectPtr<UGameplaySequenceRuntime>& R)
		{
			return !R || R->GetState() != ESequenceRuntimeState::Running;
		});

		bool bSkipThisTrigger = false;
		switch (Definition->ExecutionPolicy)
		{
		case EGameplayExecutionPolicy::IgnoreIfRunning:
			bSkipThisTrigger = RunningList.Num() > 0;
			break;
		case EGameplayExecutionPolicy::Restart:
			for (const TObjectPtr<UGameplaySequenceRuntime>& Running : RunningList)
			{
				if (Running) { Running->Cancel(); }
			}
			RunningList.Reset();
			break;
		case EGameplayExecutionPolicy::Queue:
			if (RunningList.Num() > 0)
			{
				LocalQueuedContexts.FindOrAdd(Definition).Add(Context);
				bSkipThisTrigger = true;
			}
			break;
		case EGameplayExecutionPolicy::AllowMultiple:
		default:
			break;
		}

		if (!bSkipThisTrigger)
		{
			StartLocalSequencesForDefinition(Definition, Context);
		}
	}

	// 2) Publication GLOBALE sur le Bus, en plus - pour qu'un
	// GameplayEventRegistry (via GameplayEventSubsystem) puisse réagir au
	// même tag générique si besoin, sans que ce Component ait à le savoir.
	if (UGameInstance* GameInstance = Owner->GetGameInstance())
	{
		if (UGameplayEventBus* Bus = GameInstance->GetSubsystem<UGameplayEventBus>())
		{
			Bus->Broadcast(EventTag, Context);
		}
	}
}

void UGameplayOrchestratorComponent::StartLocalSequencesForDefinition(UGameplayEventDefinition* Definition, const FEventContext& Context)
{
	AActor* Owner = GetOwner();
	UWorld* World = Owner ? Owner->GetWorld() : nullptr;
	if (!World)
	{
		return;
	}

	TArray<TObjectPtr<UGameplaySequenceRuntime>>& RunningList = LocalRunningByDefinition.FindOrAdd(Definition);

	for (const TObjectPtr<UGameplaySequence>& Sequence : Definition->Sequences)
	{
		if (!Sequence)
		{
			continue;
		}

		UGameplaySequenceRuntime* Runtime = NewObject<UGameplaySequenceRuntime>(this);
		Runtime->OnFinished.AddLambda([this, Definition](UGameplaySequenceRuntime* FinishedRuntime)
		{
			OnLocalSequenceRuntimeFinished(FinishedRuntime, Definition);
		});

		LocalActiveRuntimes.Add(Runtime);
		RunningList.Add(Runtime);

		Runtime->Start(Sequence, Context, this);
	}
}

void UGameplayOrchestratorComponent::OnLocalSequenceRuntimeFinished(UGameplaySequenceRuntime* Runtime, UGameplayEventDefinition* Definition)
{
	LocalActiveRuntimes.RemoveSingleSwap(Runtime);

	if (!Definition)
	{
		return;
	}

	if (TArray<TObjectPtr<UGameplaySequenceRuntime>>* RunningList = LocalRunningByDefinition.Find(Definition))
	{
		RunningList->RemoveSingleSwap(Runtime);
	}

	if (Definition->ExecutionPolicy == EGameplayExecutionPolicy::Queue)
	{
		const TArray<TObjectPtr<UGameplaySequenceRuntime>>* RunningList = LocalRunningByDefinition.Find(Definition);
		const bool bStillRunning = RunningList && RunningList->Num() > 0;

		if (!bStillRunning)
		{
			if (TArray<FEventContext>* Queue = LocalQueuedContexts.Find(Definition))
			{
				if (Queue->Num() > 0)
				{
					FEventContext NextContext = (*Queue)[0];
					Queue->RemoveAt(0);
					StartLocalSequencesForDefinition(Definition, NextContext);
				}
			}
		}
	}
}
