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
 */
UCLASS()
class SAVEFRAMEWORK_API USaveableRegistrySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void RegisterSaveable(USaveGuidComponent* Component);
	void UnregisterSaveable(USaveGuidComponent* Component);

	const TArray<TWeakObjectPtr<USaveGuidComponent>>& GetRegisteredComponents() const { return RegisteredComponents; }

private:
	TArray<TWeakObjectPtr<USaveGuidComponent>> RegisteredComponents;
};
