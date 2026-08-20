#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "QuestObjectiveOwnerInterface.h"
#include "QuestRuntimeState.h"
#include "QuestComponent.generated.h"

class UQuestDefinition;
class UGameplayEventBus;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestStateChanged, FName, QuestId);

/**
 * Per-player quest authority. Meant to be added to APlayerState (not the Pawn - PlayerState
 * survives possession changes and exists once per connected player, which is what quest
 * progress should be tied to).
 *
 * Server-authoritative: all quest/objective logic (starting quests, listening to the
 * GameplayEventBus, incrementing counters) only ever runs on the server. Clients only
 * ever see the replicated ActiveQuestStates snapshot - never the live UQuestObjective
 * instances, which are server-only and never cross the network.
 *
 * Same dual entry point as before: GameplayEventBus (if a definition has a StartEventTag)
 * or an explicit call (e.g. from an Orchestrator action) - both funnel into StartQuest(),
 * both server-only.
 */
UCLASS(ClassGroup = (Quest), meta = (BlueprintSpawnableComponent))
class QUESTSYSTEM_API UQuestComponent : public UActorComponent, public IQuestObjectiveOwnerInterface
{
	GENERATED_BODY()

public:

	UQuestComponent();

	//~ UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UActorComponent interface

	/** Server-only. Makes a quest definition known to this component and subscribes to its
	 *  StartEventTag on the Bus if it has one. Call once per definition, e.g. from a level
	 *  startup actor or GameMode, same idea as GameplayEventRegistryComponent. */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void RegisterQuestDefinition(UQuestDefinition* Definition);

	/** Server-only (has no effect if called on a client - use a server RPC from gameplay code if needed). */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool StartQuestById(FName QuestId);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool StartQuest(UQuestDefinition* Definition);

	/** Safe to call on client or server - reads the replicated snapshot. */
	UFUNCTION(BlueprintPure, Category = "Quest")
	bool GetQuestState(FName QuestId, FQuestRuntimeState& OutState) const;

	UFUNCTION(BlueprintPure, Category = "Quest")
	const TArray<FQuestRuntimeState>& GetActiveQuestStates() const { return ActiveQuestStates; }

	/** Looks up a registered definition by QuestId - for UI that needs the title/description text,
	 *  which the replicated runtime state doesn't carry (it's static content, not per-player progress).
	 *  Works on both server and client: RegisterQuestDefinition is meant to be called locally on both,
	 *  since it's just loading the same packaged DataAsset, not something that needs to cross the network. */
	UFUNCTION(BlueprintPure, Category = "Quest")
	UQuestDefinition* GetQuestDefinition(FName QuestId) const;

	//~ IQuestObjectiveOwnerInterface (server-only, since only the server ever creates live instances)
	virtual void OnObjectiveCompleted(UQuestObjective* Objective) override;
	virtual void OnObjectiveFailed(UQuestObjective* Objective) override;
	virtual void OnObjectiveProgressChanged(UQuestObjective* Objective) override;
	//~ End IQuestObjectiveOwnerInterface

	/** Fires on both server and clients whenever a quest's replicated state changes. Bind the journal UI to this. */
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestStateChanged OnQuestStateChanged;

protected:

	void ActivateEligibleObjectives(FQuestRuntimeState& QuestState);
	void CheckQuestCompletion(FQuestRuntimeState& QuestState);
	bool FindOwningEntry(UQuestObjective* Objective, FName& OutQuestId, FName& OutObjectiveId);

	UFUNCTION()
	void OnRep_ActiveQuestStates();

	/** Replicated snapshot - flat data only, safe to send over the network. */
	UPROPERTY(ReplicatedUsing = OnRep_ActiveQuestStates)
	TArray<FQuestRuntimeState> ActiveQuestStates;

	/** Server-only. Definitions registered via RegisterQuestDefinition - not replicated, since both
	 *  server and client load the same DataAssets locally from packaged content; only QuestId needs
	 *  to cross the wire (inside ActiveQuestStates), not the definition itself. */
	UPROPERTY()
	TMap<FName, TObjectPtr<UQuestDefinition>> AvailableQuests;

	/** Server-only. The actual running UQuestObjective instances - never replicated, never touched on a client. */
	TMap<TObjectPtr<UQuestObjective>, TPair<FName, FName>> ServerOnlyLiveInstances;

	TMap<FGameplayTag, FDelegateHandle> BusSubscriptions;
};
