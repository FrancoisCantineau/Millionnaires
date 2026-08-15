#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GlobalSaveableRegistrySubsystem.generated.h"

/**
 * Registry for session-long systems that need persistence but aren't tied
 * to a placed actor with a GUID — a quest manager, an economy system, a
 * procedural map generator's result. Unlike USaveableRegistrySubsystem
 * (actors, which load/unload), entries here are expected to live for the
 * whole session — register once (e.g. in Initialize()), never unregister.
 *
 * Reuses ISaveable — a "global saveable" and an actor's saveable state are
 * the same contract (CaptureState/RestoreState), just keyed differently:
 * an actor is identified by FGuid (from SaveGuidComponent), a global
 * system by a unique FName chosen at registration.
 */
UCLASS()
class SAVEFRAMEWORK_API UGlobalSaveableRegistrySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Object must implement ISaveable, or it will be silently skipped by SaveGame()/LoadGame(). Key must be unique project-wide. */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void RegisterGlobalSaveable(FName Key, UObject* Object);
	UFUNCTION(BlueprintCallable, Category = "Save")
	void UnregisterGlobalSaveable(FName Key);

	const TMap<FName, TWeakObjectPtr<UObject>>& GetRegistered() const { return Registered; }

private:
	TMap<FName, TWeakObjectPtr<UObject>> Registered;
};
