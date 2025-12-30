/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionDetailsWidget" - Source
 * Notes: Base widget for the mission details menu (pauses mission time while open).
 */
#include "Dispatch/UI/Widgets/Missions/Details/DispatchMissionDetailsWidget.h"

#include "Dispatch/Missions/DispatchMissionDefinition.h"

void UDispatchMissionDetailsWidget::SetFromOffer(const FGuid& InOfferId, const FDispatchMissionOffer& Offer)
{
    offerId = InOfferId;
    missionId.Invalidate();

    if (Offer.definition)
    {
        titleText = Offer.definition->title;
        descriptionText = Offer.definition->description;
        iconTexture = Offer.definition->icon;
    }
    else
    {
        titleText = FText::FromString(TEXT("Mission"));
        descriptionText = FText();
        iconTexture = nullptr;
    }

    BP_OnDataUpdated();
}

void UDispatchMissionDetailsWidget::SetFromMission(const FGuid& InMissionId, const FDispatchActiveMission& Mission)
{
    missionId = InMissionId;
    offerId.Invalidate();

    if (Mission.definition)
    {
        titleText = Mission.definition->title;
        descriptionText = Mission.definition->description;
        iconTexture = Mission.definition->icon;
    }
    else
    {
        titleText = FText::FromString(TEXT("Mission"));
        descriptionText = FText();
        iconTexture = nullptr;
    }

    BP_OnDataUpdated();
}

void UDispatchMissionDetailsWidget::RequestClose()
{
    OnCloseRequested.Broadcast();
}
