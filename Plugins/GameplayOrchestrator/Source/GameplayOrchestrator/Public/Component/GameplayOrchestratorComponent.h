#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Event/GameplayEventDefinition.h"
#include "GameplayOrchestratorComponent.generated.h"

/**
 * Generic component to place on actors (door, lamp, lever, trigger box,...).
 * Usable for a blueprint or C++ actor. 
 *
 * Quick explanation for global vs local event. 
 * - Global : going through UGameplayEventRegistry + UGameplayEventSubsystem. Actor only emitts a tag,
 * it doesn't know who is listening.
 * 
 * - Local : a mecanic for itself. Doesn't need to go through a shared registry.
 */
 
UCLASS(ClassGroup = (GameplayOrchestrator), meta = (BlueprintSpawnableComponent))
class GAMEPLAYORCHESTRATOR_API UGameplayOrchestratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGameplayOrchestratorComponent();

	/** Identity tag for the WorldStateSubSystem. Empty = no persistant state for this actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Orchestrator")
	FGameplayTag StateIdentityTag;

	/**
	 * Local events for THIS actor only. Not shared by Registry.
	 * Evaluated directly by EmitEvent (no bus subscription).
	 */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Gameplay Orchestrator")
	TArray<TObjectPtr<UGameplayEventDefinition>> LocalEvents;

	/**
	 * Publish the event and evaluates immediatly the corresponding local events for this actor.
	 * Publication on the Bus remains, to allow a global GameplayEventRegistry to react to the same generic tag.
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Orchestrator")
	void EmitEvent(FGameplayTag EventTag, AActor* Instigator = nullptr);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TSet<TObjectPtr<UGameplayEventDefinition>> LocalFiredOneShot;

	UPROPERTY()
	TArray<TObjectPtr<class UGameplaySequenceRuntime>> LocalActiveRuntimes;

	TMap<TObjectPtr<UGameplayEventDefinition>, TArray<TObjectPtr<class UGameplaySequenceRuntime>>> LocalRunningByDefinition;
	TMap<TObjectPtr<UGameplayEventDefinition>, TArray<FEventContext>> LocalQueuedContexts;

	void StartLocalSequencesForDefinition(UGameplayEventDefinition* Definition, const FEventContext& Context);
	void OnLocalSequenceRuntimeFinished(class UGameplaySequenceRuntime* Runtime, UGameplayEventDefinition* Definition);
};
