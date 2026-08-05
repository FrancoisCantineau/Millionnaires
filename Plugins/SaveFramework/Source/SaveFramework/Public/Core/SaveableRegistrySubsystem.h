#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveableRegistrySubsystem.generated.h"

class USaveGuidComponent;

/**
 * Tracks which SaveGuidComponents are currently loaded, so other systems
 * (the save subsystem, the world state) never have to search the world.
 * Registration is automatic: SaveGuidComponent registers itself in
 * BeginPlay and unregisters in EndPlay.
 *
 * This subsystem knows nothing about WHAT is registered, only THAT it is.
 *
 * Keeps both a flat list (full iteration — used when something needs
 * "every loaded saveable", e.g. SaveGame()) and a GUID-keyed map (O(1)
 * lookup — used when something needs "the one loaded saveable with this
 * GUID", e.g. WorldState applying a change to a specific actor). Populated
 * together, always in sync.
 */
UCLASS()
class SAVEFRAMEWORK_API USaveableRegistrySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void RegisterSaveable(USaveGuidComponent* Component);
	void UnregisterSaveable(USaveGuidComponent* Component);

	/** O(1) — the loaded component with this GUID, or nullptr if none is currently loaded. */
	USaveGuidComponent* FindByGuid(const FGuid& TargetId) const;

	/** Full list — for operations that genuinely need every loaded saveable (e.g. capturing a save). */
	const TArray<TWeakObjectPtr<USaveGuidComponent>>& GetRegisteredComponents() const { return RegisteredComponents; }

private:
	TArray<TWeakObjectPtr<USaveGuidComponent>> RegisteredComponents;
	TMap<FGuid, TWeakObjectPtr<USaveGuidComponent>> ComponentsByGuid;
};
