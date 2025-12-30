/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchGameMode" - Header
 * Notes: GameMode used for the Dispatch control room. Prevents spawning an unwanted default Pawn at PlayerStart.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DispatchGameMode.generated.h"

class AController;
class AActor;
class APawn;

/**
 * GameMode for the Dispatch control room.
 * - Uses the custom DispatchPlayerController.
 * - Optionally prevents spawning the default Pawn at PlayerStart (camera-only dispatch).
 */
UCLASS()
class MILLIONNAIRES_API ADispatchGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
#pragma region LIFECYCLE

    /** Default constructor. */
    ADispatchGameMode();

#pragma endregion LIFECYCLE

protected:
#pragma region STARTUP

    /** If true, the GameMode will not spawn a default Pawn at PlayerStart (avoids the invisible pawn at runtime). */
    UPROPERTY(EditAnywhere, Category = "Dispatch|Startup")
    bool bDisableDefaultPawnSpawn = true;

    /** Override used to prevent spawning an unwanted Pawn at PlayerStart in Dispatch mode. */
    virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

#pragma endregion STARTUP
};
