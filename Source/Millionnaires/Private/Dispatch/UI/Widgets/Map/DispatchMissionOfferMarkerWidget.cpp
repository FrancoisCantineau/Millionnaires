/* 
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionOfferMarkerWidget" - Source
 * Notes: Base widget for mission offer markers shown on the map.
 */
#include "Dispatch/UI/Widgets/Map/DispatchMissionOfferMarkerWidget.h"

#include "Components/Button.h"
#include "Input/Reply.h"

#pragma region API

void UDispatchMissionOfferMarkerWidget::InitializeMarker(const FDispatchMapOfferMarker& InData)
{
    data = InData;
    HandleMarkerDataApplied(data);
}

void UDispatchMissionOfferMarkerWidget::SetSelected(bool bInSelected)
{
    bIsSelected = bInSelected;
    HandleSelectedChanged(bIsSelected);
}

#pragma endregion API

#pragma region UUSERWIDGET

void UDispatchMissionOfferMarkerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (clickButton)
    {
        clickButton->OnClicked.AddDynamic(this, &UDispatchMissionOfferMarkerWidget::HandleButtonClicked);
    }
}

FReply UDispatchMissionOfferMarkerWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        OnMarkerClicked.Broadcast(data.offerId);
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

#pragma endregion UUSERWIDGET

#pragma region INTERNAL

void UDispatchMissionOfferMarkerWidget::HandleButtonClicked()
{
    OnMarkerClicked.Broadcast(data.offerId);
}

#pragma endregion INTERNAL
