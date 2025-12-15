/* 
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionOfferDetailsWidget" - Source
 * Notes: Base widget for mission details panel inside the map.
 */
#include "Dispatch/UI/Widgets/Map/DispatchMissionOfferDetailsWidget.h"

#include "Components/Button.h"

#pragma region API

void UDispatchMissionOfferDetailsWidget::SetOfferSummary(const FDispatchMissionOfferSummary& InSummary)
{
    summary = InSummary;
    HandleSummaryApplied(summary);
    RequestVisible(true);
}

#pragma endregion API

#pragma region UUSERWIDGET

void UDispatchMissionOfferDetailsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (acceptButton)
    {
        acceptButton->OnClicked.AddDynamic(this, &UDispatchMissionOfferDetailsWidget::HandleAcceptClicked);
    }

    if (declineButton)
    {
        declineButton->OnClicked.AddDynamic(this, &UDispatchMissionOfferDetailsWidget::HandleDeclineClicked);
    }
}

#pragma endregion UUSERWIDGET

#pragma region INTERNAL

void UDispatchMissionOfferDetailsWidget::HandleAcceptClicked()
{
    OnAcceptRequested.Broadcast(summary.offerId);
}

void UDispatchMissionOfferDetailsWidget::HandleDeclineClicked()
{
    OnDeclineRequested.Broadcast(summary.offerId);
}

#pragma endregion INTERNAL
