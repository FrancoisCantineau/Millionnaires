#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "StatefulActorInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UStatefulActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implémentée par tout Actor dont l'état doit être piloté par le
 * WorldStateSubsystem (portes, lampes, leviers...).
 *
 * BlueprintNativeEvent => implémentable en C++ pur (override *_Implementation)
 * OU en Blueprint, sans jamais nécessiter d'enfant BP pour une classe C++ native.
 *
 * Règle d'or inchangée depuis la V1 : l'Actor ne modifie JAMAIS l'état d'un
 * autre Actor directement. Il reçoit son état via ApplyState (poussé par le
 * Subsystem s'il est déjà enregistré/chargé, ou lu par lui-même au BeginPlay
 * via GameplayOrchestratorComponent).
 */
class GAMEPLAYORCHESTRATOR_API IStatefulActor
{
	GENERATED_BODY()

public:
	// NB : UHT n'autorise pas les BlueprintNativeEvent 'const' - la version V1 avait
	// cette const par erreur, corrigé ici.
	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay Orchestrator|Stateful Actor")
	FGameplayTag GetStateIdentityTag();

	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay Orchestrator|Stateful Actor")
	void ApplyState(FGameplayTag NewStateTag);
};
