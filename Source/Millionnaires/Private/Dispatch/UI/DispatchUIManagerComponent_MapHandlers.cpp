/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchUIManagerComponent_MapHandlers" - Source
 * Notes: Implements map-offer callbacks declared in DispatchUIManagerComponent.h (fixes linker errors).
 */
#include "Dispatch/UI/DispatchUIManagerComponent.h"

#include "Dispatch/Missions/DispatchMissionManagerComponent.h"

#pragma region MAP_OFFER_CALLBACKS

void UDispatchUIManagerComponent::HandleOfferDeclinedFromMap(const FGuid& OfferId)
{
    // Even with map-as-camera, we keep this callback to support future UI (2D/3D notifications).
    if (missionManager.IsValid())
    {
        missionManager->DeclineOffer(OfferId);
    }
}

void UDispatchUIManagerComponent::HandleOfferAcceptedFromMap(const FGuid& OfferId)
{
    // UI must pick agents (multi for Dispatch mission / single for FPS mission later),
    // then call TryAcceptOffer(OfferId, Agents).
    OnOfferAcceptRequested.Broadcast(OfferId);
}

#pragma endregion MAP_OFFER_CALLBACKS
