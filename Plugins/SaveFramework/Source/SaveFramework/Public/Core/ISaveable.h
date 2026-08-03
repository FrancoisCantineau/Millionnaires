#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StructUtils/InstancedStruct.h"
#include "ISaveable.generated.h"

/**
 * Contract between gameplay and the save system. This is the ONLY thing
 * the plugin knows about a "saveable" object — it never knows the concrete
 * class implementing it.
 *
 * FInstancedStruct lets each actor return ANY UStruct it wants (its own
 * FDoorSaveData, FEnemySaveData, ...) without the plugin needing to know
 * that struct type ahead of time. As long as the struct's fields are
 * UPROPERTY, serialization "just works" via the engine's built-in reflection.
 *
 * STABILITY NOTE: CaptureState()/RestoreState() are the public contract.
 * The internal representation (FInstancedStruct today) can be swapped later
 * for a custom FArchive-based format — e.g. if network replication, diffing,
 * or fine-grained compression become real requirements — without breaking
 * any gameplay code that implements this interface. Don't make that change
 * until one of those needs actually shows up on a real project.
 *
 * Identity (GUID) is NOT part of this interface — it always comes from a
 * USaveGuidComponent on the actor, whether or not that actor also
 * implements ISaveable. Implement this interface only if the actor has
 * CUSTOM state beyond position/active (HP, inventory, quest flags...).
 */
UINTERFACE(MinimalAPI, Blueprintable)
class USaveable : public UInterface
{
	GENERATED_BODY()
};

class SAVEFRAMEWORK_API ISaveable
{
	GENERATED_BODY()

public:
	/** Return this actor's current state as a generic struct. Keep it data-only — no pointers to other actors. */
	UFUNCTION(BlueprintNativeEvent, Category = "SaveFramework")
	FInstancedStruct CaptureState() const;

	/** Apply previously captured state. Called after the actor exists (BeginPlay or later), never before. */
	UFUNCTION(BlueprintNativeEvent, Category = "SaveFramework")
	void RestoreState(const FInstancedStruct& InState);
};
