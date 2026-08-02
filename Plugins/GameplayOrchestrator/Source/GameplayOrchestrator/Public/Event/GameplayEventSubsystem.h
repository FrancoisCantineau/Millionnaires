#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/GameplayEventContext.h"
#include "GameplayEventSubsystem.generated.h"

class UGameplayEventDefinition;
class UGameplayEventRegistry;
class UGameplaySequenceRuntime;

/**
 * Une Definition référencée par plusieurs Registries (ex: un DataAsset
 * DA_BossReveal utilisé à la fois dans Registry_Chapter1 et Registry_TestMap)
 * doit être trackée comme "one-shot" INDÉPENDAMMENT par Registry - sinon la
 * consommer dans un Registry la désactive à tort dans l'autre.
 */
struct FRegisteredEventEntry
{
	TWeakObjectPtr<UGameplayEventRegistry> Registry;
	TWeakObjectPtr<UGameplayEventDefinition> Definition;
};

struct FFiredOneShotKey
{
	TWeakObjectPtr<UGameplayEventRegistry> Registry;
	TWeakObjectPtr<UGameplayEventDefinition> Definition;

	bool operator==(const FFiredOneShotKey& Other) const
	{
		return Registry == Other.Registry && Definition == Other.Definition;
	}
};

FORCEINLINE uint32 GetTypeHash(const FFiredOneShotKey& Key)
{
	return HashCombine(GetTypeHash(Key.Registry), GetTypeHash(Key.Definition));
}

/**
 * Chef d'orchestre du système d'events globaux.
 *
 *   RegisterEventRegistry(Registry) -> indexe chaque Definition par son
 *   EventTag, s'abonne au Bus pour chaque tag encore jamais vu.
 *
 *   [le Bus reçoit un event] -> OnBusEvent -> pour chaque Definition
 *   indexée sous ce tag (triées par Priority) :
 *     - skip si bOneShot déjà consommée (clé Registry+Definition)
 *     - EvaluateConditions
 *     - applique ExecutionPolicy (AllowMultiple/IgnoreIfRunning/Restart/Queue)
 *       par rapport aux GameplaySequenceRuntime déjà actifs pour cette
 *       Definition
 *     - si autorisé : crée un UGameplaySequenceRuntime par Sequence de la
 *       Definition, le garde dans ActiveRuntimes (UPROPERTY, sinon le GC le
 *       détruirait en plein milieu d'une attente latente)
 */
UCLASS()
class GAMEPLAYORCHESTRATOR_API UGameplayEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay Orchestrator|Event Subsystem")
	void RegisterEventRegistry(UGameplayEventRegistry* Registry);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Orchestrator|Event Subsystem")
	void UnregisterEventRegistry(UGameplayEventRegistry* Registry);

	/** Réinitialise le suivi "one-shot" (utile par exemple lors d'un New Game / New Game+). */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Orchestrator|Event Subsystem")
	void ResetOneShotTracking();

	/** Annule proprement tous les Runtimes actifs (timers/abonnements) à l'arrêt du jeu/PIE. */
	virtual void Deinitialize() override;

private:
	void OnBusEvent(const FEventContext& Context);
	void StartSequencesForDefinition(UGameplayEventDefinition* Definition, const FEventContext& Context);
	void OnSequenceRuntimeFinished(UGameplaySequenceRuntime* Runtime, TWeakObjectPtr<UGameplayEventDefinition> DefinitionWeak);

	// Tag -> entrées (Registry+Definition) indexées sous ce tag.
	TMap<FGameplayTag, TArray<FRegisteredEventEntry>> DefinitionsByTag;

	TMap<FGameplayTag, int32> SubscriptionRefCount;
	TMap<FGameplayTag, FDelegateHandle> BusSubscriptions;

	TSet<FFiredOneShotKey> FiredOneShotDefinitions;

	/** UPROPERTY indispensable : sans ça, le GC détruirait un Runtime en pleine attente latente (timer, event). */
	UPROPERTY()
	TArray<TObjectPtr<UGameplaySequenceRuntime>> ActiveRuntimes;

	/** Runtimes actuellement en cours, groupés par Definition - sert à appliquer ExecutionPolicy. */
	TMap<TWeakObjectPtr<UGameplayEventDefinition>, TArray<TObjectPtr<UGameplaySequenceRuntime>>> RuntimesByDefinition;

	/** File d'attente pour EGameplayExecutionPolicy::Queue - un contexte en attente par Definition tant qu'une exécution tourne. */
	TMap<TWeakObjectPtr<UGameplayEventDefinition>, TArray<FEventContext>> QueuedContextsByDefinition;
};
