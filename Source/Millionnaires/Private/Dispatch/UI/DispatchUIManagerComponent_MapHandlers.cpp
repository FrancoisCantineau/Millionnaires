/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchUIManagerComponent_MapHandlers" - Source
 * Notes: Fixes missing linker symbols for map callbacks declared as UFUNCTION in DispatchUIManagerComponent.h.
 */
#include "Dispatch/UI/DispatchUIManagerComponent.h"

#include "Dispatch/Missions/DispatchMissionManagerComponent.h"

#pragma region MAP_CALLBACKS

void UDispatchUIManagerComponent::HandleOfferDeclinedFromMap(const FGuid& OfferId)
{
    if (!missionManager.IsValid())
    {
        return;
    }

    missionManager->DeclineOffer(OfferId);
}

void UDispatchUIManagerComponent::HandleOfferAcceptedFromMap(const FGuid& OfferId)
{
    // IMPORTANT: UI must pick agents, then call TryAcceptOffer(OfferId, Agents).
    OnOfferAcceptRequested.Broadcast(OfferId);
}

#pragma endregion MAP_CALLBACKS
