/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionOfferClickProxyComponent" - Source
 * Notes: Lightweight component attached to clickable actors to open mission UI (offer or mission).
 */
#include "Dispatch/Map/DispatchMissionOfferClickProxyComponent.h"

#include "Dispatch/Map/DispatchMissionSiteActor.h"

void UDispatchMissionOfferClickProxyComponent::SetOffer(const FGuid& OfferId, ADispatchMissionSiteActor* Site)
{
    kind = EDispatchMissionClickProxyKind::Offer;
    offerId = OfferId;
    missionId.Invalidate();
    site = Site;
}

void UDispatchMissionOfferClickProxyComponent::SetMission(const FGuid& MissionId, ADispatchMissionSiteActor* Site)
{
    kind = EDispatchMissionClickProxyKind::Mission;
    missionId = MissionId;
    offerId.Invalidate();
    site = Site;
}

void UDispatchMissionOfferClickProxyComponent::Clear()
{
    kind = EDispatchMissionClickProxyKind::None;
    offerId.Invalidate();
    missionId.Invalidate();
    site.Reset();
}
