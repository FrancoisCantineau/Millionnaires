#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InstancedStruct.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/SaveableSubsystemInterface.h"
#include "GameplayWorldStateSubsystem.generated.h"

/**
 * Politique de persistance d'un état.
 *
 * RuntimeOnly : survit au streaming (World Partition/Level Streaming) mais
 *   JAMAIS inclus dans CaptureState - donc perdu à un rechargement de
 *   sauvegarde. Pour des états transitoires (ex: variation cosmétique locale).
 * SaveGame : inclus dans CaptureState, restauré normalement au chargement.
 *   C'est le cas d'usage par défaut pour la grande majorité des états
 *   (portes, lumières, flags de mission).
 * Permanent : inclus dans CaptureState comme SaveGame. Sémantiquement réservé
 *   par convention aux états qui ne doivent JAMAIS être remis à zéro même en
 *   New Game+ ou reset partiel - MAIS le plugin ne fait aucune distinction de
 *   comportement entre SaveGame et Permanent au niveau de Capture/Restore :
 *   cette nuance est réservée à VOTRE logique de Save Manager (ex: filtrer
 *   les tags Permanent différemment lors d'un reset). Documentée ici pour
 *   éviter toute confusion, pas pour promettre un comportement que le
 *   plugin n'implémente pas lui-même.
 */
UENUM(BlueprintType)
enum class EWorldStatePersistence : uint8
{
	RuntimeOnly,
	SaveGame,
	Permanent
};

/** Snapshot sérialisable de tout le contenu sauvegardable du GameplayWorldStateSubsystem. */
USTRUCT()
struct GAMEPLAYORCHESTRATOR_API FWorldStateSnapshot
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FGameplayTag, FGameplayTag> States;

	UPROPERTY()
	TMap<FGameplayTag, FInstancedStruct> ExtendedStates;

	// La policy elle-même est aussi sauvegardée, pour qu'un état créé avec une
	// policy donnée la conserve après un rechargement (plutôt que de retomber
	// sur le défaut si le code de gameplay qui l'a fixée n'est pas rejoué).
	UPROPERTY()
	TMap<FGameplayTag, EWorldStatePersistence> Policies;
};

/**
 * Source de vérité unique pour l'état persistant du MONDE (portes, lampes,
 * leviers, flags de mission...), découplée de l'existence en mémoire des
 * Actors. Renommé depuis "GameStateSubsystem" (V1) pour éviter la confusion
 * avec AGameStateBase, qui désigne un concept Unreal différent.
 *
 * Règle d'or : une Action écrit TOUJOURS ici, jamais directement sur un Actor.
 * Un Actor lit TOUJOURS son état d'ici à son BeginPlay, jamais l'inverse.
 *
 * GameInstanceSubsystem => persiste indépendamment du streaming des niveaux.
 * N'écrit JAMAIS sur disque lui-même (voir ISaveableSubsystem).
 */
UCLASS()
class GAMEPLAYORCHESTRATOR_API UGameplayWorldStateSubsystem : public UGameInstanceSubsystem, public ISaveableSubsystem
{
	GENERATED_BODY()

public:
	// --- État simple (discret) : Tag -> Tag ---------------------------------

	/**
	 * Définit l'état associé à Key. Si des Actors sont actuellement enregistrés
	 * pour ce tag, ils sont notifiés immédiatement. Sinon, l'état est stocké et
	 * sera lu par l'Actor concerné à son prochain BeginPlay.
	 * Persistence par défaut = SaveGame (le cas le plus courant).
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Orchestrator|World State")
	void SetState(FGameplayTag Key, FGameplayTag Value, EWorldStatePersistence Persistence = EWorldStatePersistence::SaveGame);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay Orchestrator|World State")
	FGameplayTag GetState(FGameplayTag Key) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay Orchestrator|World State")
	bool HasState(FGameplayTag Key) const;

	// --- État riche (optionnel) : Tag -> FInstancedStruct -------------------

	UFUNCTION(BlueprintCallable, Category = "Gameplay Orchestrator|World State")
	void SetExtendedState(FGameplayTag Key, const FInstancedStruct& Value, EWorldStatePersistence Persistence = EWorldStatePersistence::SaveGame);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Orchestrator|World State")
	FInstancedStruct GetExtendedState(FGameplayTag Key) const;

	// --- Registre des Actors chargés par tag --------------------------------

	/** Appelé par UGameplayOrchestratorComponent au BeginPlay de l'Actor. */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Orchestrator|World State")
	void RegisterActor(FGameplayTag IdentityTag, AActor* Actor);

	/** Appelé au EndPlay/déchargement - indispensable pour ne pas accumuler de refs mortes. */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Orchestrator|World State")
	void UnregisterActor(FGameplayTag IdentityTag, AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay Orchestrator|World State")
	TArray<AActor*> GetRegisteredActors(FGameplayTag IdentityTag) const;

	// --- ISaveableSubsystem --------------------------------------------------

	virtual FGameplayTag GetSaveSectionId() const override;
	virtual FInstancedStruct CaptureState() const override;
	virtual void RestoreState(const FInstancedStruct& InData) override;

	// --- Debug ---------------------------------------------------------------

	FString DebugDumpStates() const;

private:
	UPROPERTY()
	TMap<FGameplayTag, FGameplayTag> States;

	UPROPERTY()
	TMap<FGameplayTag, FInstancedStruct> ExtendedStates;

	UPROPERTY()
	TMap<FGameplayTag, EWorldStatePersistence> Policies;

	TMultiMap<FGameplayTag, TWeakObjectPtr<AActor>> RegisteredActors;

	void NotifyRegisteredActors(FGameplayTag Key, FGameplayTag NewValue);
};
