#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Interface.h"
#include "SaveableSubsystemInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class USaveableSubsystem : public UInterface
{
	GENERATED_BODY()
};

/**
 * Contrat générique implémenté par tout subsystem qui doit survivre à une
 * sauvegarde/rechargement (WorldStateSubsystem en fait partie, mais rien
 * n'empêche votre inventaire, vos quêtes, etc. de l'implémenter aussi).
 *
 * Ce plugin n'écrit JAMAIS sur disque lui-même. Un Save Manager côté PROJET
 * interroge tous les subsystems implémentant cette interface et agrège leurs
 * données. Voir Examples/ExampleSaveManager.
 */
class GAMEPLAYORCHESTRATOR_API ISaveableSubsystem
{
	GENERATED_BODY()

public:
	virtual FGameplayTag GetSaveSectionId() const = 0;
	virtual FInstancedStruct CaptureState() const = 0;
	virtual void RestoreState(const FInstancedStruct& InData) = 0;
};
