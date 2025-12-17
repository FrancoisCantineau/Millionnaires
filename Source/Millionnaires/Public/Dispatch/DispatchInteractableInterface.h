/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchInteractableInterface" - Header
 * Notes: Interface for clickable / hoverable objects inside the Dispatch control room.
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DispatchInteractableInterface.generated.h"

class APlayerController;

UINTERFACE(BlueprintType)
class MILLIONNAIRES_API UDispatchInteractableInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface used by any actor that should react to hover and click events in the Dispatch room.
 */
class MILLIONNAIRES_API IDispatchInteractableInterface
{
    GENERATED_BODY()

public:
    /** Called when the dispatch cursor starts hovering this actor. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dispatch|Interaction")
    void OnDispatchHoverStarted(APlayerController* Controller);

    /** Called when the dispatch cursor stops hovering this actor. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dispatch|Interaction")
    void OnDispatchHoverEnded(APlayerController* Controller);

    /** Called when the dispatch cursor clicks on this actor. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dispatch|Interaction")
    void OnDispatchClicked(APlayerController* Controller);
};
