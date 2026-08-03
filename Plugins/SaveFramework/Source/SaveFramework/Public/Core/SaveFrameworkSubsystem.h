#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveFrameworkSubsystem.generated.h"

/**
 * Single public entry point for the rest of the game. Nothing else in a
 * project should touch USaveFrameworkSaveGame or ISaveable::Execute_* directly —
 * call SaveGame()/LoadGame() here instead.
 *
 * This starter version is synchronous, on purpose, to keep the first
 * version simple and easy to debug. Swap SaveGameToSlot/LoadGameFromSlot
 * for AsyncSaveGameToSlot/AsyncLoadGameFromSlot once it's working, so a
 * save never freezes the game.
 */
UCLASS()
class SAVEFRAMEWORK_API USaveFrameworkSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SaveFramework")
	void SaveGame(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "SaveFramework")
	void LoadGame(const FString& SlotName);
};
