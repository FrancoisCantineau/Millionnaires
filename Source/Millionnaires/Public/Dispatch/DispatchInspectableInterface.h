/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchInspectableInterface" - Header
 * Notes: Interface for characters or actors that can be "inspected" when time is slowed / stopped in Dispatch.
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DispatchInspectableInterface.generated.h"

UINTERFACE(BlueprintType)
class UDispatchInspectableInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for actors that expose information in Dispatch when the cursor focuses them and time stops.
 */
class IDispatchInspectableInterface
{
    GENERATED_BODY()

public:
    /** Called when the Dispatch cursor starts focusing this actor as the primary inspectable target. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dispatch|Inspect")
    void OnDispatchInspectStarted(APlayerController* Controller);

    /** Called when the Dispatch cursor stops focusing this actor (time is resuming or cursor moved away). */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dispatch|Inspect")
    void OnDispatchInspectEnded(APlayerController* Controller);
};
