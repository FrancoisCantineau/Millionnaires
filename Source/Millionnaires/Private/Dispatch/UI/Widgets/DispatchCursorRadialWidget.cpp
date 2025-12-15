/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchCursorRadialWidget" - Source
 * Notes: Base widget for cursor radial (progress + screen position + fade in/out).
 */
#include "Dispatch/UI/Widgets/DispatchCursorRadialWidget.h"

#pragma region API

void UDispatchCursorRadialWidget::SetProgress(float InProgress)
{
    CurrentProgress = FMath::Clamp(InProgress, 0.f, 1.f);
    HandleProgressChanged(CurrentProgress);
}

void UDispatchCursorRadialWidget::SetScreenPosition(const FVector2D& InScreenPosition)
{
    // Center on cursor.
    SetAlignmentInViewport(FVector2D(0.5f, 0.5f));

    // GetMousePosition() returns raw pixels; UMG uses DPI-scaled Slate units.
    // bRemoveDPIScale=true converts pixel coordinates into correct viewport units.
    SetPositionInViewport(InScreenPosition, /*bRemoveDPIScale*/ true);
}

void UDispatchCursorRadialWidget::RequestVisible(bool bVisible)
{
    bTargetVisible = bVisible;

    if (bTargetVisible)
    {
        // Ensure visible during fade in.
        if (GetVisibility() == ESlateVisibility::Hidden)
        {
            SetVisibility(ESlateVisibility::HitTestInvisible);
        }
    }
}

#pragma endregion API

#pragma region UUSERWIDGET

void UDispatchCursorRadialWidget::NativeConstruct()
{
    Super::NativeConstruct();

    CurrentOpacity = 0.f;
    SetRenderOpacity(CurrentOpacity);
    SetVisibility(ESlateVisibility::Hidden);

    bTargetVisible = false;
}

void UDispatchCursorRadialWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    const float TargetOpacity = bTargetVisible ? 1.f : 0.f;
    const float Speed = bTargetVisible ? FadeInSpeed : FadeOutSpeed;

    CurrentOpacity = FMath::FInterpTo(CurrentOpacity, TargetOpacity, InDeltaTime, Speed);
    CurrentOpacity = FMath::Clamp(CurrentOpacity, 0.f, 1.f);

    SetRenderOpacity(CurrentOpacity);

    // Hide once fully faded out.
    if (!bTargetVisible && CurrentOpacity <= HideThreshold)
    {
        SetVisibility(ESlateVisibility::Hidden);
    }
    else if (bTargetVisible)
    {
        if (GetVisibility() == ESlateVisibility::Hidden)
        {
            SetVisibility(ESlateVisibility::HitTestInvisible);
        }
    }
}

#pragma endregion UUSERWIDGET
