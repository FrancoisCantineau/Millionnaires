#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "QuestSystemInterface.generated.h"

/** 
 * Interface for the quest system.
 * TODO: Supprimer cette interface une fois le système de dispatch implémenté si il reste que le debug.
 */
UINTERFACE(MinimalAPI, BlueprintType)
class UQuestSystemInterface : public UInterface
{
	GENERATED_BODY()
};

class IQuestSystemInterface
{
	GENERATED_BODY()

public:
    
	/* Start a debug objective by its class name */
	virtual void StartDebugObjective(const FString& ObjectiveClassName) = 0;
};