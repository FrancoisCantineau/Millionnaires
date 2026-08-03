#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "StructUtils/InstancedStruct.h"
#include "SaveFrameworkSaveGame.generated.h"

/** One actor's captured state, keyed by its stable GUID in the map below. */
USTRUCT()
struct FSaveFrameworkRecord
{
	GENERATED_BODY()

	// Generic part — always captured for every registered actor, no code required.
	UPROPERTY()
	FTransform Transform = FTransform::Identity;

	UPROPERTY()
	bool bActive = true;

	// Custom part — only populated if the actor implements ISaveable.
	UPROPERTY()
	FInstancedStruct State;
};

/**
 * Pure data container — no logic. Extend this later by splitting Records
 * into separate versioned blocks (Inventory, Quests...) once a single flat
 * map stops being enough.
 */
UCLASS()
class SAVEFRAMEWORK_API USaveFrameworkSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 SaveVersion = 1;

	UPROPERTY()
	TMap<FGuid, FSaveFrameworkRecord> Records;
};
