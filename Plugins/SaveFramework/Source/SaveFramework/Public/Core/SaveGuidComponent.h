#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SaveGuidComponent.generated.h"

/**
 * Drop this on any actor to give it a stable identity (GUID) across
 * sessions AND generic persistence behaviour (position, active/visible)
 * handled automatically — no custom code required.
 *
 * For a placed actor, the GUID is generated once, in the editor, and
 * stored with the level. Actors spawned dynamically at runtime need a
 * different identity strategy (a "spawn record"), not covered yet.
 *
 * Custom state (HP, inventory, quest flags...) is a separate concern,
 * handled by ISaveable on the actor itself if it needs it. This
 * component only ever deals with the generic, universal part.
 *
 * bAutoTrackGenericState decides whether a snapshot of raw transform/active
 * makes sense as "the truth" for this actor:
 *  - true (default) — fine for anything whose current transform IS its
 *    whole state: a door, a pushed crate, static set dressing. A raw
 *    position snapshot is always meaningful for these.
 *  - false — for an actor whose transform is mid-something and would be
 *    misleading as a snapshot (a platform 40% through a 5-minute trip). Its
 *    real state belongs in ISaveable/CaptureState instead (e.g. "target
 *    node + policy"), which the actor interprets itself in RestoreState.
 */
UCLASS(ClassGroup = (SaveFramework), meta = (BlueprintSpawnableComponent))
class SAVEFRAMEWORK_API USaveGuidComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USaveGuidComponent();

	UFUNCTION(BlueprintPure, Category = "SaveFramework")
	FGuid GetSaveId() const { return SaveId; }

	/** Same as GetSaveId(), as readable text — handy to Print String it and copy it for a manual test. */
	UFUNCTION(BlueprintPure, Category = "SaveFramework")
	FString GetSaveIdString() const { return SaveId.ToString(); }

	/** Moves the owner. Called either immediately by USaveFrameworkWorldState (actor already loaded), or by this component itself on load if a move was pending. */
	void ApplyTransform(const FTransform& NewTransform);

	/** Toggles owner visibility + collision. Same dual call path as ApplyTransform. */
	void ApplyActive(bool bNewActive);

	/**
	 * TEST-ONLY. Simulates this actor unloading — deposits its current
	 * state into the WorldState, WITHOUT touching level streaming or
	 * actually ending play. Use this + SimulateReload to test the
	 * deposit/claim round trip in isolation, inside a single PIE session,
	 * with no streaming setup required. Not meant for real gameplay use.
	 */
	UFUNCTION(BlueprintCallable, Category = "SaveFramework|Debug")
	void SimulateUnload();

	/**
	 * TEST-ONLY. Simulates this actor reloading — claims whatever state is
	 * waiting for it in the WorldState and applies it, as BeginPlay would.
	 */
	UFUNCTION(BlueprintCallable, Category = "SaveFramework|Debug")
	void SimulateReload();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "SaveFramework")
	void EnsureValidSaveId();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveFramework",SaveGame)
	FGuid SaveId;

	/** See class comment. Turn off for actors whose raw transform mid-EndPlay wouldn't mean anything on its own. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SaveFramework")
	bool bAutoTrackGenericState = true;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Shared by EndPlay and SimulateUnload — deposits current state into the WorldState. */
	void DepositStateIntoWorldState();

	/** Shared by BeginPlay and SimulateReload — claims and applies any state waiting in the WorldState. */
	void ClaimStateFromWorldState();

	

protected:
#if WITH_EDITOR
	virtual void OnRegister() override;
	void EnsurePersistentSaveId();
#endif
};
