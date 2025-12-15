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
