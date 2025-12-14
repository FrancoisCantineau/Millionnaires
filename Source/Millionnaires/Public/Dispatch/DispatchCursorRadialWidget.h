/**
 * Millionnaires Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchCursorRadialWidget"
 * Notes: Simple wrapper around a UUserWidget used to display a radial fill around the mouse cursor.
 */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DispatchCursorRadialWidget.generated.h"

/**
 * Widget used by the Dispatch player controller to show a radial fill around the mouse cursor.
 * The visual implementation (material, image, progress binding) is meant to be done in Blueprint.
 */
UCLASS()
class MILLIONNAIRES_API UDispatchCursorRadialWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Sets the current progress in [0,1] and notifies Blueprints. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Cursor")
    void SetProgress(float InProgress);

    /** Moves the widget so that it is centered on the given screen position. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Cursor")
    void SetScreenPosition(const FVector2D& InScreenPosition);

protected:
    /** Current radial progress (0 = empty, 1 = full). */
    UPROPERTY(BlueprintReadOnly, Category = "Dispatch|Cursor")
    float CurrentProgress = 0.f;

    /** Implement this in Blueprint to drive your actual radial material / image. */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dispatch|Cursor")
    void HandleProgressChanged(float InProgress);
};
