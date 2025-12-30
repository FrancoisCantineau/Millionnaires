/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchGameMode" - Source
 * Notes: Implementation of the Dispatch GameMode.
 */

#include "Dispatch/DispatchGameMode.h"
#include "Dispatch/DispatchPlayerController.h"

ADispatchGameMode::ADispatchGameMode()
{
    PlayerControllerClass = ADispatchPlayerController::StaticClass();
}

#pragma region STARTUP

APawn* ADispatchGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
    if (bDisableDefaultPawnSpawn)
    {
        UE_LOG(LogTemp, Log, TEXT("[DispatchGameMode] Default pawn spawn skipped (Dispatch is camera-only)."));
        return nullptr;
    }

    return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}

#pragma endregion STARTUP
