/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionOfferClickProxyComponent" - Header
 * Notes: Lightweight component attached to clickable actors to open mission UI (offer or mission).
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Dispatch/Missions/DispatchMissionTypes.h"
#include "DispatchMissionOfferClickProxyComponent.generated.h"

class ADispatchMissionSiteActor;

#pragma region DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDispatchClickProxyTriggered, EDispatchMissionClickProxyKind, Kind, const FGuid&, Id);

#pragma endregion DELEGATES

/**
 * Attach to any actor you want to be clickable for mission UI.
 * The DispatchCursorComponent should fire OnActorClicked(AActor*),
 * and the UI manager resolves this component to decide what to open.
 */
UCLASS(ClassGroup=(Dispatch), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UDispatchMissionOfferClickProxyComponent : public UActorComponent
{
    GENERATED_BODY()

public:
#pragma region API

    /// <summary>Configures this proxy as an Offer click.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|ClickProxy")
    void SetOffer(const FGuid& OfferId, ADispatchMissionSiteActor* Site);

    /// <summary>Configures this proxy as a Mission click.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|ClickProxy")
    void SetMission(const FGuid& MissionId, ADispatchMissionSiteActor* Site);

    /// <summary>Clears the proxy state.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|ClickProxy")
    void Clear();

    /// <summary>Returns the kind (Offer / Mission / None).</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|Map|ClickProxy")
    EDispatchMissionClickProxyKind GetKind() const { return kind; }

    /// <summary>Returns the current offer id (valid only if Kind==Offer).</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|Map|ClickProxy")
    FGuid GetOfferId() const { return offerId; }

    /// <summary>Returns the current mission id (valid only if Kind==Mission).</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|Map|ClickProxy")
    FGuid GetMissionId() const { return missionId; }

    /// <summary>Returns the linked site actor (optional).</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|Map|ClickProxy")
    ADispatchMissionSiteActor* GetSite() const { return site.Get(); }

#pragma endregion API

#pragma region EVENTS

    /// <summary>Optional: can be manually triggered by BP if you want click logic inside the proxy.</summary>
    UPROPERTY(BlueprintAssignable, Category="Dispatch|Map|ClickProxy")
    FDispatchClickProxyTriggered OnTriggered;

#pragma endregion EVENTS

private:
#pragma region STATE

    UPROPERTY(VisibleAnywhere, Category="Dispatch|Map|ClickProxy")
    EDispatchMissionClickProxyKind kind = EDispatchMissionClickProxyKind::None;

    UPROPERTY(VisibleAnywhere, Category="Dispatch|Map|ClickProxy")
    FGuid offerId;

    UPROPERTY(VisibleAnywhere, Category="Dispatch|Map|ClickProxy")
    FGuid missionId;

    TWeakObjectPtr<ADispatchMissionSiteActor> site;

#pragma endregion STATE
};
