/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchGameMode" - Header
 * Notes: Optional GameMode used when the player is inside the Dispatch control room map.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DispatchGameMode.generated.h"

/**
 * GameMode for the Dispatch control room. It sets the custom DispatchPlayerController by default.
 */
UCLASS()
class MILLIONNAIRES_API ADispatchGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    /** Default constructor. */
    ADispatchGameMode();
};
