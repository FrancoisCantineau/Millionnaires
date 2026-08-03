#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StructUtils/InstancedStruct.h"
#include "SaveFrameworkWorldState.generated.h"

class USaveGuidComponent;

/** Generic state any saveable actor has: where it is, and whether it's active. */
USTRUCT()
struct FSaveableGenericState
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform Transform = FTransform::Identity;

	UPROPERTY()
	bool bActive = true;
};

/**
 * Partial update to FSaveableGenericState — only the fields with their
 * bHasX flag set are applied. Lets callers change just the transform, just
 * bActive, or both, through a single function instead of one setter per
 * field.
 */
USTRUCT(BlueprintType)
struct FSaveableStatePatch
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "SaveFramework")
	bool bHasTransform = false;

	UPROPERTY(BlueprintReadWrite, Category = "SaveFramework")
	FTransform Transform = FTransform::Identity;

	UPROPERTY(BlueprintReadWrite, Category = "SaveFramework")
	bool bHasActive = false;

	UPROPERTY(BlueprintReadWrite, Category = "SaveFramework")
	bool bActive = true;

	static FSaveableStatePatch MakeTransform(const FTransform& InTransform)
	{
		FSaveableStatePatch Patch;
		Patch.bHasTransform = true;
		Patch.Transform = InTransform;
		return Patch;
	}

	static FSaveableStatePatch MakeActive(bool bInActive)
	{
		FSaveableStatePatch Patch;
		Patch.bHasActive = true;
		Patch.bActive = bInActive;
		return Patch;
	}
};

/**
 * Single entry point for systems (like an event orchestrator) that need to
 * change an actor's state by GUID, without ever needing to know whether
 * that actor is currently loaded.
 *
 * Two channels, deliberately kept separate:
 *  - Generic (SetState): position + active/visible, true of every saveable
 *    actor, handled entirely by SaveGuidComponent — no game code required.
 *  - Custom (ApplyCustomState): anything gameplay-specific (a cart's
 *    destination and travel duration, a door's "open" flag...), carried as
 *    an opaque FInstancedStruct and handed to the actor's own ISaveable
 *    implementation. This subsystem never inspects what's inside — the
 *    actor decides what to DO with a state change (play a Timeline, a
 *    Niagara effect, a sound...), this subsystem only ever stores and
 *    delivers it.
 *
 * Deliberately NOT one function per property or per behaviour — SetState
 * is a single setter, not an action. "How" a change is realised (snap vs.
 * interpolate) is SaveGuidComponent's call (ApplyTransform/ApplyActive are
 * overridable), never this subsystem's.
 *
 * - Actor loaded (registered in USaveableRegistrySubsystem) → change is
 *   applied immediately.
 * - Actor not loaded → the change is stored here and applied automatically
 *   by SaveGuidComponent the next time that actor loads (BeginPlay).
 */
UCLASS()
class SAVEFRAMEWORK_API USaveFrameworkWorldState : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SaveFramework")
	void SetState(const FGuid& TargetId, const FSaveableStatePatch& Patch);

	/**
	 * Generic escape hatch for anything gameplay-specific. The actor must
	 * implement ISaveable — CustomState is handed to its RestoreState()
	 * exactly like a normal load would.
	 */
	UFUNCTION(BlueprintCallable, Category = "SaveFramework")
	void ApplyCustomState(const FGuid& TargetId, const FInstancedStruct& CustomState);

	/** Called by SaveGuidComponent when it loads. Returns true and fills OutState if a generic change was pending (and consumes it). */
	bool ConsumePendingState(const FGuid& TargetId, FSaveableGenericState& OutState);

	/** Same as above, for a pending custom state. */
	bool ConsumePendingCustomState(const FGuid& TargetId, FInstancedStruct& OutState);

	/** Read-only access so USaveFrameworkSubsystem can persist pending changes even for actors that haven't loaded yet. */
	const TMap<FGuid, FSaveableGenericState>& GetAllPendingStates() const { return PendingStates; }
	const TMap<FGuid, FInstancedStruct>& GetAllPendingCustomStates() const { return PendingCustomStates; }

private:
	USaveGuidComponent* FindLoadedComponent(const FGuid& TargetId) const;

	TMap<FGuid, FSaveableGenericState> PendingStates;
	TMap<FGuid, FInstancedStruct> PendingCustomStates;
};
