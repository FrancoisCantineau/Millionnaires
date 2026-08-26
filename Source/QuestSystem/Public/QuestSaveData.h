#pragma once

#include "CoreMinimal.h"
#include "QuestRuntimeState.h"
#include "QuestSaveData.generated.h"

/**
 * QuestSystem's own save contract - deliberately self-contained, no dependency on any
 * particular save plugin. A project wires this up to whatever save system it uses via a
 * small adapter class living in the PROJECT (not in this plugin, not in the save plugin) -
 * see UQuestComponent::CaptureSaveData/RestoreSaveData.
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEM_API FQuestSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	TArray<FQuestRuntimeState> Quests;
};
