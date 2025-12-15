/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchFadableWidget" - Source
 * Notes: Implements fade in/out for UMG widgets.
 */
#include "Dispatch/UI/Widgets/DispatchFadableWidget.h"

void UDispatchFadableWidget::RequestVisible(bool bVisible)
{
    bTargetVisible = bVisible;

    if (bTargetVisible)
    {
        if (GetVisibility() == ESlateVisibility::Hidden)
        {
            SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void UDispatchFadableWidget::NativeConstruct()
{
    Super::NativeConstruct();

    CurrentOpacity = 0.f;
    SetRenderOpacity(CurrentOpacity);
    SetVisibility(ESlateVisibility::Hidden);

    bTargetVisible = false;
}

void UDispatchFadableWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    const float TargetOpacity = bTargetVisible ? 1.f : 0.f;
    const float Speed = bTargetVisible ? FadeInSpeed : FadeOutSpeed;

    CurrentOpacity = FMath::FInterpTo(CurrentOpacity, TargetOpacity, InDeltaTime, Speed);
    CurrentOpacity = FMath::Clamp(CurrentOpacity, 0.f, 1.f);
    SetRenderOpacity(CurrentOpacity);

    if (!bTargetVisible && CurrentOpacity <= HideThreshold)
    {
        SetVisibility(ESlateVisibility::Hidden);
    }
    else if (bTargetVisible && GetVisibility() == ESlateVisibility::Hidden)
    {
        SetVisibility(ESlateVisibility::Visible);
    }
}
