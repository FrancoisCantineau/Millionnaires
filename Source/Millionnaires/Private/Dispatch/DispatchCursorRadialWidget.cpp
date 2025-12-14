/**
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchCursorRadialWidget"
 * Notes: UUserWidget helper for radial cursor fill.
 */

#include "Dispatch/DispatchCursorRadialWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"

void UDispatchCursorRadialWidget::SetProgress(float InProgress)
{
    CurrentProgress = FMath::Clamp(InProgress, 0.f, 1.f);
    HandleProgressChanged(CurrentProgress);
}

void UDispatchCursorRadialWidget::SetScreenPosition(const FVector2D& InScreenPosition)
{
    SetPositionInViewport(InScreenPosition, true);
}
