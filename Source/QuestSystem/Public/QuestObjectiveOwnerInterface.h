#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "QuestObjectiveOwnerInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UQuestObjectiveOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implemented by whatever drives a UQuestObjective's lifecycle (UQuestComponent,
 * added to a player's PlayerState). An objective only ever
 * talks to this interface, never to a concrete owner class.
 */
class QUESTSYSTEM_API IQuestObjectiveOwnerInterface
{
	GENERATED_BODY()

public:
	virtual void OnObjectiveCompleted(class UQuestObjective* Objective) = 0;
	virtual void OnObjectiveFailed(class UQuestObjective* Objective) = 0;

	/** For progress that changed WITHOUT completing/failing (e.g. a Counter going from 1/3 to 2/3).
	 *  Completion/failure already imply a state change on their own - this is for everything else
	 *  that should still refresh the replicated snapshot and notify UI. */
	virtual void OnObjectiveProgressChanged(class UQuestObjective* Objective) = 0;
};
