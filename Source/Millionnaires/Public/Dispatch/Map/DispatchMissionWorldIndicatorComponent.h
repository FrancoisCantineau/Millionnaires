/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionWorldIndicatorComponent" - Header
 * Notes: Binds to DispatchMissionManagerComponent and drives world indicators (site highlight).
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Dispatch/Missions/DispatchMissionTypes.h"
#include "DispatchMissionWorldIndicatorComponent.generated.h"

class UDispatchMissionManagerComponent;
class ADispatchMissionSiteActor;

/**
 * World indicator driver for the Dispatch phase.
 * - Highlights a MissionSiteActor when an offer appears.
 * - Keeps / updates highlight while a mission is running.
 * - Restores materials when offer expires/declined OR mission ends.
 *
 * IMPORTANT:
 * This component must exist at runtime (recommended Owner: Dispatch PlayerController).
 */
UCLASS(ClassGroup=(Dispatch), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UDispatchMissionWorldIndicatorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
#pragma region LIFECYCLE

    UDispatchMissionWorldIndicatorComponent();

protected:
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

    void BindToMissionManager();
    ADispatchMissionSiteActor* ResolveSiteFromOffer(const FGuid& OfferId);
    ADispatchMissionSiteActor* ResolveSiteFromMission(const FGuid& MissionId);

    /** Uses reflection to read optional properties (works even if your struct/DA was changed in another zip). */
    bool TryReadOptionalOfferLocationActor(const UScriptStruct* OfferStruct, const void* OfferPtr, ADispatchMissionSiteActor*& OutSite) const;
    bool TryReadOptionalMissionLocationValue(const UObject* Obj, const FName PropertyName, int64& OutValue) const;

    /** Picks a site by mission location if available; otherwise falls back to nearest by worldLocation. */
    ADispatchMissionSiteActor* PickBestSite(const FVector& WorldLocation, const int64* DesiredLocationOrNull, int32 Seed) const;

#pragma endregion INTERNAL

private:
#pragma region STATE

    /** Cached mission manager reference. */
    TWeakObjectPtr<UDispatchMissionManagerComponent> missionManager;

    /** OfferId -> Site mapping (cached when offer is added). */
    TMap<FGuid, TWeakObjectPtr<ADispatchMissionSiteActor>> offerToSite;

    /** MissionId -> Site mapping (cached when mission starts). */
    TMap<FGuid, TWeakObjectPtr<ADispatchMissionSiteActor>> missionToSite;

#pragma endregion STATE
};
