#include "QuestComponent.h"
#include "QuestDefinition.h"
#include "Event/GameplayEventBus.h"
#include "Core/GameplayEventContext.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"

UQuestComponent::UQuestComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UQuestComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UQuestComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr)
		{
			if (UGameplayEventBus* Bus = GameInstance->GetSubsystem<UGameplayEventBus>())
			{
				for (const TPair<FGameplayTag, FDelegateHandle>& Pair : BusSubscriptions)
				{
					Bus->Unsubscribe(Pair.Key, Pair.Value);
				}
			}
		}
	}
	BusSubscriptions.Empty();

	Super::EndPlay(EndPlayReason);
}

void UQuestComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UQuestComponent, ActiveQuestStates);
}

void UQuestComponent::RegisterQuestDefinition(UQuestDefinition* Definition)
{
	// Definitions themselves are just local content (packaged DataAssets) - registering them
	// only matters for server-side bus subscriptions and quest-start lookups, so both server
	// and clients call this locally; only the resulting ActiveQuestStates snapshot replicates.
	if (!Definition || Definition->QuestId.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("UQuestComponent::RegisterQuestDefinition - invalid definition or missing QuestId"));
		return;
	}

	AvailableQuests.Add(Definition->QuestId, Definition);

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (Definition->StartEventTag.IsValid())
	{
		if (UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr)
		{
			if (UGameplayEventBus* Bus = GameInstance->GetSubsystem<UGameplayEventBus>())
			{
				FName QuestId = Definition->QuestId;
				FDelegateHandle Handle = Bus->SubscribeNative(Definition->StartEventTag, [this, QuestId](const FEventContext&)
				{
					StartQuestById(QuestId);
				});
				BusSubscriptions.Add(Definition->StartEventTag, Handle);
			}
		}
	}
}

bool UQuestComponent::StartQuestById(FName QuestId)
{
	if (TObjectPtr<UQuestDefinition>* Found = AvailableQuests.Find(QuestId))
	{
		return StartQuest(*Found);
	}

	UE_LOG(LogTemp, Warning, TEXT("UQuestComponent::StartQuestById - '%s' was never registered via RegisterQuestDefinition"), *QuestId.ToString());
	return false;
}

bool UQuestComponent::StartQuest(UQuestDefinition* Definition)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("UQuestComponent::StartQuest - called on a client, ignored. Route this through a server RPC in gameplay code."));
		return false;
	}

	if (!Definition)
	{
		return false;
	}

	for (const FQuestRuntimeState& Existing : ActiveQuestStates)
	{
		if (Existing.QuestId == Definition->QuestId)
		{
			UE_LOG(LogTemp, Warning, TEXT("UQuestComponent::StartQuest - '%s' is already started"), *Definition->QuestId.ToString());
			return false;
		}
	}

	FQuestRuntimeState NewState;
	NewState.QuestId = Definition->QuestId;
	NewState.State = EQuestState::Active;

	for (const FQuestObjectiveEntry& Entry : Definition->Objectives)
	{
		FQuestObjectiveRuntimeState ObjState;
		ObjState.ObjectiveId = Entry.ObjectiveId;
		ObjState.State = EQuestObjectiveState::Inactive;
		NewState.Objectives.Add(ObjState);
	}

	int32 Index = ActiveQuestStates.Add(NewState);
	ActivateEligibleObjectives(ActiveQuestStates[Index]);

	OnQuestStateChanged.Broadcast(Definition->QuestId);
	return true;
}

void UQuestComponent::ActivateEligibleObjectives(FQuestRuntimeState& QuestState)
{
	TObjectPtr<UQuestDefinition>* DefinitionPtr = AvailableQuests.Find(QuestState.QuestId);
	if (!DefinitionPtr || !*DefinitionPtr)
	{
		return;
	}
	UQuestDefinition* Definition = *DefinitionPtr;

	for (FQuestObjectiveRuntimeState& ObjState : QuestState.Objectives)
	{
		if (ObjState.State != EQuestObjectiveState::Inactive)
		{
			continue;
		}

		const FQuestObjectiveEntry* Entry = Definition->Objectives.FindByPredicate(
			[&ObjState](const FQuestObjectiveEntry& E) { return E.ObjectiveId == ObjState.ObjectiveId; });
		if (!Entry || !Entry->ObjectiveTemplate)
		{
			continue;
		}

		bool bDependenciesMet = true;
		for (const FName& DepId : Entry->Dependencies)
		{
			const FQuestObjectiveRuntimeState* DepState = QuestState.Objectives.FindByPredicate(
				[&DepId](const FQuestObjectiveRuntimeState& O) { return O.ObjectiveId == DepId; });
			if (!DepState || DepState->State != EQuestObjectiveState::Completed)
			{
				bDependenciesMet = false;
				break;
			}
		}

		if (!bDependenciesMet)
		{
			continue;
		}

		UQuestObjective* Instance = DuplicateObject<UQuestObjective>(Entry->ObjectiveTemplate, this);
		Instance->Initialize(this);
		Instance->ObjectiveActivate();

		ServerOnlyLiveInstances.Add(Instance, TPair<FName, FName>(QuestState.QuestId, ObjState.ObjectiveId));

		ObjState.State = EQuestObjectiveState::Active;
		ObjState.ProgressCurrent = Instance->GetProgressCurrent();
		ObjState.ProgressTarget = Instance->GetProgressTarget();
		ObjState.DisplayFields = Instance->GetDisplayFields();
		ObjState.ActivationServerTime = Instance->HasTimeLimit() && GetWorld() ? GetWorld()->GetTimeSeconds() : -1.0f;
	}
}

bool UQuestComponent::FindOwningEntry(UQuestObjective* Objective, FName& OutQuestId, FName& OutObjectiveId)
{
	if (const TPair<FName, FName>* Found = ServerOnlyLiveInstances.Find(Objective))
	{
		OutQuestId = Found->Key;
		OutObjectiveId = Found->Value;
		return true;
	}
	return false;
}

void UQuestComponent::OnObjectiveCompleted(UQuestObjective* Objective)
{
	FName QuestId, ObjectiveId;
	if (!FindOwningEntry(Objective, QuestId, ObjectiveId))
	{
		return;
	}
	ServerOnlyLiveInstances.Remove(Objective);

	FQuestRuntimeState* QuestState = ActiveQuestStates.FindByPredicate(
		[&QuestId](const FQuestRuntimeState& Q) { return Q.QuestId == QuestId; });
	if (!QuestState)
	{
		return;
	}

	FQuestObjectiveRuntimeState* ObjState = QuestState->Objectives.FindByPredicate(
		[&ObjectiveId](const FQuestObjectiveRuntimeState& O) { return O.ObjectiveId == ObjectiveId; });
	if (ObjState)
	{
		ObjState->State = EQuestObjectiveState::Completed;
		ObjState->ProgressCurrent = Objective->GetProgressCurrent();
		ObjState->ProgressTarget = Objective->GetProgressTarget();
		ObjState->DisplayFields = Objective->GetDisplayFields();
	}

	ActivateEligibleObjectives(*QuestState);
	CheckQuestCompletion(*QuestState);

	OnQuestStateChanged.Broadcast(QuestId);
}

void UQuestComponent::OnObjectiveProgressChanged(UQuestObjective* Objective)
{
	FName QuestId, ObjectiveId;
	if (!FindOwningEntry(Objective, QuestId, ObjectiveId))
	{
		return;
	}

	FQuestRuntimeState* QuestState = ActiveQuestStates.FindByPredicate(
		[&QuestId](const FQuestRuntimeState& Q) { return Q.QuestId == QuestId; });
	if (!QuestState)
	{
		return;
	}

	FQuestObjectiveRuntimeState* ObjState = QuestState->Objectives.FindByPredicate(
		[&ObjectiveId](const FQuestObjectiveRuntimeState& O) { return O.ObjectiveId == ObjectiveId; });
	if (ObjState)
	{
		// Deliberately only the display-facing snapshot - never State, Dependencies, or
		// completion logic. Those only ever change via OnObjectiveCompleted/OnObjectiveFailed.
		ObjState->ProgressCurrent = Objective->GetProgressCurrent();
		ObjState->ProgressTarget = Objective->GetProgressTarget();
		ObjState->DisplayFields = Objective->GetDisplayFields();
	}

	OnQuestStateChanged.Broadcast(QuestId);
}

void UQuestComponent::OnObjectiveFailed(UQuestObjective* Objective)
{
	FName QuestId, ObjectiveId;
	if (!FindOwningEntry(Objective, QuestId, ObjectiveId))
	{
		return;
	}
	ServerOnlyLiveInstances.Remove(Objective);

	FQuestRuntimeState* QuestState = ActiveQuestStates.FindByPredicate(
		[&QuestId](const FQuestRuntimeState& Q) { return Q.QuestId == QuestId; });
	if (!QuestState)
	{
		return;
	}

	FQuestObjectiveRuntimeState* ObjState = QuestState->Objectives.FindByPredicate(
		[&ObjectiveId](const FQuestObjectiveRuntimeState& O) { return O.ObjectiveId == ObjectiveId; });
	if (ObjState)
	{
		ObjState->State = EQuestObjectiveState::Failed;
	}

	// Same simplification as before, carried over: any failed objective fails the whole quest.
	QuestState->State = EQuestState::Failed;

	OnQuestStateChanged.Broadcast(QuestId);
}

void UQuestComponent::CheckQuestCompletion(FQuestRuntimeState& QuestState)
{
	for (const FQuestObjectiveRuntimeState& ObjState : QuestState.Objectives)
	{
		if (ObjState.State != EQuestObjectiveState::Completed)
		{
			return;
		}
	}
	QuestState.State = EQuestState::Completed;
}

bool UQuestComponent::GetQuestState(FName QuestId, FQuestRuntimeState& OutState) const
{
	if (const FQuestRuntimeState* Found = ActiveQuestStates.FindByPredicate(
		[&QuestId](const FQuestRuntimeState& Q) { return Q.QuestId == QuestId; }))
	{
		OutState = *Found;
		return true;
	}
	return false;
}

UQuestDefinition* UQuestComponent::GetQuestDefinition(FName QuestId) const
{
	if (const TObjectPtr<UQuestDefinition>* Found = AvailableQuests.Find(QuestId))
	{
		return *Found;
	}
	return nullptr;
}

void UQuestComponent::OnRep_ActiveQuestStates()
{
	// Fires on clients whenever the whole array changes. We don't know which specific quest
	// changed from this alone (no per-element RepNotify without a FastArraySerializer), so
	// broadcast a wildcard-ish refresh: UI bound to this should just re-pull GetActiveQuestStates().
	// Known limitation, not built: a proper FFastArraySerializer would give per-quest deltas
	// and be cheaper on the wire - worth it once there are many simultaneous quests.
	for (const FQuestRuntimeState& QuestState : ActiveQuestStates)
	{
		OnQuestStateChanged.Broadcast(QuestState.QuestId);
	}
}
