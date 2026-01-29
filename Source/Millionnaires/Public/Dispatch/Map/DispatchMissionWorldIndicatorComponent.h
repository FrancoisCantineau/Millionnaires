/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionWorldIndicatorComponent" - Header
 * Notes: Binds to DispatchMissionManagerComponent and drives world indicators (site highlight + world notifications).
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Dispatch/Missions/DispatchMissionTypes.h"
#include "DispatchMissionWorldIndicatorComponent.generated.h"

class UDispatchMissionManagerComponent;
class ADispatchMissionSiteActor;
class ADispatchMissionNotificationPoint;
class ADispatchMissionOfferIndicatorActor;

/**
 * World indicator driver for the Dispatch phase.
 * - Highlights a MissionSiteActor when an offer appears.
 * - Spawns a world-space UMG indicator on a NotificationPoint matching the site location.
 * - Adds a ClickProxy component on highlighted actors so clicks can open mission details.
 * - Keeps / updates highlight while a mission is running.
 * - Restores materials when offer expires/declined OR mission ends.
 *
 * Recommended Owner: Dispatch PlayerController (same owner as UDispatchMissionManagerComponent & UDispatchUIManagerComponent).
 */
UCLASS(ClassGroup=(Dispatch), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UDispatchMissionWorldIndicatorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
#pragma region LIFECYCLE

    /** Constructor. */
    UDispatchMissionWorldIndicatorComponent();

protected:
    /** Called on start. */
    virtual void BeginPlay() override;

#pragma endregion LIFECYCLE

public:
#pragma region SETTINGS

    /** If true, prints logs to help diagnose binding and highlight resolution. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Debug")
    bool bDebugLogs = true;

    /** If true, tries to find the MissionManagerComponent anywhere in the world if it isn't on the same owner. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Debug")
    bool bFallbackSearchWorldForMissionManager = true;

    /** If set, spawns a world-space offer indicator actor when an offer appears. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|WorldNotifications")
    TSubclassOf<ADispatchMissionOfferIndicatorActor> offerIndicatorActorClass;

    /** Z offset applied to spawned world notifications. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|WorldNotifications")
    float offerIndicatorZOffset = 25.f;

    /** If true, attaches ClickProxy components to highlighted target actors so clicking them can open the mission menu. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|WorldNotifications")
    bool bAttachClickProxiesToHighlightedActors = true;

#pragma endregion SETTINGS

protected:
#pragma region CALLBACKS

    UFUNCTION()
    void HandleOfferAdded(const FGuid& OfferId);

    UFUNCTION()
    void HandleOfferRemoved(const FGuid& OfferId);

    UFUNCTION()
    void HandleMissionStateChanged(const FGuid& MissionId, EDispatchMissionState NewState);

#pragma endregion CALLBACKS

protected:
#pragma region INTERNAL

    /** Binds to mission manager events. */
    void BindToMissionManager();

    /** Resolves the MissionSiteActor for a given offer. */
    ADispatchMissionSiteActor* ResolveSiteFromOffer(const FGuid& OfferId, FDispatchMissionOffer* OutOffer = nullptr);

    /** Resolves the MissionSiteActor for a given mission. */
    ADispatchMissionSiteActor* ResolveSiteFromMission(const FGuid& MissionId, FDispatchActiveMission* OutMission = nullptr);

    /** Picks the best site (exact location > any location > nearest). */
    ADispatchMissionSiteActor* PickBestSite(EDispatchMissionLocation DesiredLocation, const FVector& NearWorldLocation, int32 Seed) const;

    /** Picks a notification point for a given location and reference position. */
    ADispatchMissionNotificationPoint* PickNotificationPoint(EDispatchMissionLocation Location, const FVector& NearWorldLocation, int32 Seed) const;

    /** Spawns/destroys the world-space offer indicator. */
    void SpawnOfferIndicator(const FGuid& OfferId, const FVector& WorldLocation, EDispatchMissionLocation Location, int32 Seed);
    void DestroyOfferIndicator(const FGuid& OfferId);

    /** Updates ClickProxy state on the site + its target actors. */
    void SetClickProxyForSite(ADispatchMissionSiteActor* Site, const FGuid* OfferIdOrNull, const FGuid* MissionIdOrNull);

#pragma endregion INTERNAL

private:
#pragma region STATE

    /** Cached mission manager reference. */
    TWeakObjectPtr<UDispatchMissionManagerComponent> missionManager;

    /** OfferId -> Site mapping (cached when offer is added). */
    TMap<FGuid, TWeakObjectPtr<ADispatchMissionSiteActor>> offerToSite;

    /** MissionId -> Site mapping (cached when mission starts). */
    TMap<FGuid, TWeakObjectPtr<ADispatchMissionSiteActor>> missionToSite;

    /** OfferId -> spawned indicator actor. */
    TMap<FGuid, TWeakObjectPtr<ADispatchMissionOfferIndicatorActor>> offerToIndicator;

#pragma endregion STATE
};
