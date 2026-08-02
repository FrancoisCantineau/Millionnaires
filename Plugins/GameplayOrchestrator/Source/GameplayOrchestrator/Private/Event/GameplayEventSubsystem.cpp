#include "Event/GameplayEventSubsystem.h"
#include "Event/GameplayEventDefinition.h"
#include "Event/GameplayEventRegistry.h"
#include "Event/GameplayEventBus.h"
#include "Sequence/GameplaySequence.h"
#include "Sequence/GameplaySequenceRuntime.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Algo/Sort.h"

void UGameplayEventSubsystem::RegisterEventRegistry(UGameplayEventRegistry* Registry)
{
	if (!Registry)
	{
		return;
	}

	UGameplayEventBus* Bus = GetGameInstance()->GetSubsystem<UGameplayEventBus>();
	if (!Bus)
	{
		return;
	}

	for (const TObjectPtr<UGameplayEventDefinition>& Definition : Registry->Events)
	{
		if (!Definition || !Definition->EventTag.IsValid())
		{
			continue;
		}

		FRegisteredEventEntry Entry;
		Entry.Registry = Registry;
		Entry.Definition = Definition;
		DefinitionsByTag.FindOrAdd(Definition->EventTag).Add(Entry);

		int32& RefCount = SubscriptionRefCount.FindOrAdd(Definition->EventTag);
		if (RefCount == 0)
		{
			FGameplayTag TagCopy = Definition->EventTag;
			FDelegateHandle Handle = Bus->SubscribeNative(TagCopy, [this](const FEventContext& Context)
			{
				OnBusEvent(Context);
			});
			BusSubscriptions.Add(TagCopy, Handle);
		}
		++RefCount;
	}
}

void UGameplayEventSubsystem::UnregisterEventRegistry(UGameplayEventRegistry* Registry)
{
	if (!Registry)
	{
		return;
	}

	UGameplayEventBus* Bus = GetGameInstance() ? GetGameInstance()->GetSubsystem<UGameplayEventBus>() : nullptr;

	for (const TObjectPtr<UGameplayEventDefinition>& Definition : Registry->Events)
	{
		if (!Definition || !Definition->EventTag.IsValid())
		{
			continue;
		}

		if (TArray<FRegisteredEventEntry>* Entries = DefinitionsByTag.Find(Definition->EventTag))
		{
			for (int32 i = 0; i < Entries->Num(); ++i)
			{
				if ((*Entries)[i].Registry.Get() == Registry && (*Entries)[i].Definition.Get() == Definition)
				{
					Entries->RemoveAt(i);
					break;
				}
			}
			if (Entries->Num() == 0)
			{
				DefinitionsByTag.Remove(Definition->EventTag);
			}
		}

		if (int32* RefCount = SubscriptionRefCount.Find(Definition->EventTag))
		{
			--(*RefCount);
			if (*RefCount <= 0)
			{
				SubscriptionRefCount.Remove(Definition->EventTag);
				if (Bus)
				{
					if (const FDelegateHandle* Handle = BusSubscriptions.Find(Definition->EventTag))
					{
						Bus->Unsubscribe(Definition->EventTag, *Handle);
					}
				}
				BusSubscriptions.Remove(Definition->EventTag);
			}
		}
	}
}

void UGameplayEventSubsystem::ResetOneShotTracking()
{
	FiredOneShotDefinitions.Reset();
}

void UGameplayEventSubsystem::Deinitialize()
{
	// Copie défensive : Cancel() déclenche OnSequenceRuntimeFinished en
	// cascade, qui modifie ActiveRuntimes pendant qu'on itère dessus.
	TArray<TObjectPtr<UGameplaySequenceRuntime>> RuntimesToCancel = ActiveRuntimes;
	for (const TObjectPtr<UGameplaySequenceRuntime>& Runtime : RuntimesToCancel)
	{
		if (Runtime)
		{
			Runtime->Cancel();
		}
	}
	ActiveRuntimes.Reset();
	RuntimesByDefinition.Reset();
	QueuedContextsByDefinition.Reset();

	Super::Deinitialize();
}

void UGameplayEventSubsystem::OnBusEvent(const FEventContext& Context)
{
	TArray<FRegisteredEventEntry>* Entries = DefinitionsByTag.Find(Context.EventTag);
	if (!Entries)
	{
		return;
	}

	TArray<FRegisteredEventEntry> SortedEntries = *Entries;
	Algo::SortBy(SortedEntries,
		[](const FRegisteredEventEntry& E) { return E.Definition.IsValid() ? E.Definition->Priority : 0; },
		[](int32 A, int32 B) { return A > B; });

	for (const FRegisteredEventEntry& Entry : SortedEntries)
	{
		UGameplayEventDefinition* Definition = Entry.Definition.Get();
		if (!Definition)
		{
			continue;
		}

		FFiredOneShotKey OneShotKey{ Entry.Registry, Entry.Definition };
		if (Definition->bOneShot && FiredOneShotDefinitions.Contains(OneShotKey))
		{
			continue;
		}

		if (!Definition->EvaluateConditions(Context))
		{
			continue;
		}

		if (Definition->bOneShot)
		{
			FiredOneShotDefinitions.Add(OneShotKey);
		}

		// --- Application de l'ExecutionPolicy ---
		TArray<TObjectPtr<UGameplaySequenceRuntime>>& RunningList = RuntimesByDefinition.FindOrAdd(Definition);
		// Nettoyage défensif : retire les entrées qui ne seraient plus Running
		// (ne devrait normalement jamais arriver, OnSequenceRuntimeFinished les
		// retire déjà, mais on reste robuste face à un éventuel oubli).
		RunningList.RemoveAll([](const TObjectPtr<UGameplaySequenceRuntime>& R)
		{
			return !R || R->GetState() != ESequenceRuntimeState::Running;
		});

		switch (Definition->ExecutionPolicy)
		{
		case EGameplayExecutionPolicy::IgnoreIfRunning:
			if (RunningList.Num() > 0)
			{
				continue;
			}
			break;

		case EGameplayExecutionPolicy::Restart:
			for (const TObjectPtr<UGameplaySequenceRuntime>& Running : RunningList)
			{
				if (Running)
				{
					Running->Cancel();
				}
			}
			RunningList.Reset();
			break;

		case EGameplayExecutionPolicy::Queue:
			if (RunningList.Num() > 0)
			{
				QueuedContextsByDefinition.FindOrAdd(Definition).Add(Context);
				continue;
			}
			break;

		case EGameplayExecutionPolicy::AllowMultiple:
		default:
			break;
		}

		StartSequencesForDefinition(Definition, Context);
	}
}

void UGameplayEventSubsystem::StartSequencesForDefinition(UGameplayEventDefinition* Definition, const FEventContext& Context)
{
	UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	if (!World)
	{
		return;
	}

	TArray<TObjectPtr<UGameplaySequenceRuntime>>& RunningList = RuntimesByDefinition.FindOrAdd(Definition);
	TWeakObjectPtr<UGameplayEventDefinition> DefinitionWeak(Definition);

	for (const TObjectPtr<UGameplaySequence>& Sequence : Definition->Sequences)
	{
		if (!Sequence)
		{
			continue;
		}

		UGameplaySequenceRuntime* Runtime = NewObject<UGameplaySequenceRuntime>(this);
		Runtime->OnFinished.AddLambda([this, DefinitionWeak](UGameplaySequenceRuntime* FinishedRuntime)
		{
			OnSequenceRuntimeFinished(FinishedRuntime, DefinitionWeak);
		});

		ActiveRuntimes.Add(Runtime);
		RunningList.Add(Runtime);

		Runtime->Start(Sequence, Context, World);
	}
}

void UGameplayEventSubsystem::OnSequenceRuntimeFinished(UGameplaySequenceRuntime* Runtime, TWeakObjectPtr<UGameplayEventDefinition> DefinitionWeak)
{
	ActiveRuntimes.RemoveSingleSwap(Runtime);

	UGameplayEventDefinition* Definition = DefinitionWeak.Get();
	if (!Definition)
	{
		return;
	}

	if (TArray<TObjectPtr<UGameplaySequenceRuntime>>* RunningList = RuntimesByDefinition.Find(DefinitionWeak))
	{
		RunningList->RemoveSingleSwap(Runtime);
	}

	// Politique Queue : si plus rien ne tourne pour cette Definition et qu'un
	// contexte est en attente, on le démarre maintenant.
	if (Definition->ExecutionPolicy == EGameplayExecutionPolicy::Queue)
	{
		const TArray<TObjectPtr<UGameplaySequenceRuntime>>* RunningList = RuntimesByDefinition.Find(DefinitionWeak);
		const bool bStillRunning = RunningList && RunningList->Num() > 0;

		if (!bStillRunning)
		{
			if (TArray<FEventContext>* Queue = QueuedContextsByDefinition.Find(DefinitionWeak))
			{
				if (Queue->Num() > 0)
				{
					FEventContext NextContext = (*Queue)[0];
					Queue->RemoveAt(0);
					// NB : les conditions ne sont volontairement PAS ré-évaluées
					// ici - elles ont déjà été validées au moment de la mise en
					// file. Un besoin de re-validation à la sortie de file serait
					// un raffinement futur si un cas d'usage concret l'exige.
					StartSequencesForDefinition(Definition, NextContext);
				}
			}
		}
	}
}
