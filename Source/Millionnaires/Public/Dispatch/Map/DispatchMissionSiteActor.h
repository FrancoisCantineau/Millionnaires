/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionSiteActor" - Header
 * Notes: Represents a mission site in the Dispatch map and supports highlight + click targets.
 */
#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Dispatch/Missions/DispatchMissionTypes.h"
#include "DispatchMissionSiteActor.generated.h"

class UPrimitiveComponent;
class UMeshComponent;
class UMaterialInterface;

/**
 * Mission site actor placed in the level.
 * - Holds a MissionLocation dropdown (used to match offers and notification points).
 * - Holds TargetActors that should be highlighted / clickable when an offer or mission is active.
 * - Supports OverlayMaterial highlight (preferred for this project) and/or CustomDepth highlight.
 */
UCLASS()
class MILLIONNAIRES_API ADispatchMissionSiteActor : public AActor
{
    GENERATED_BODY()

public:
#pragma region LIFECYCLE

    /// <summary>Constructor.</summary>
    ADispatchMissionSiteActor();

#pragma endregion LIFECYCLE

public:
#pragma region SETTINGS

    /// <summary>Location used to match mission offers and notification points.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Site")
    EDispatchMissionLocation missionLocation = EDispatchMissionLocation::Any;

    /// <summary>Actors to highlight & make clickable when this site has an active offer/mission.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Site")
    TArray<TObjectPtr<AActor>> targetActors;

    /// <summary>If true, highlights by setting the OverlayMaterial on mesh components.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Highlight")
    bool bUseOverlayMaterialHighlight = true;

    /// <summary>Material to apply in the OverlayMaterial slot when highlighted.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Highlight")
    TObjectPtr<UMaterialInterface> overlayHighlightMaterial = nullptr;

    /// <summary>Material to apply in the OverlayMaterial slot for the short expiry pulse (offer expired without accept).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Highlight")
    TObjectPtr<UMaterialInterface> offerExpiredPulseOverlayMaterial = nullptr;

    /// <summary>Duration (seconds) of the short expiry pulse highlight.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Highlight", meta=(ClampMin="0.01", ClampMax="5.0"))
    float offerExpiredPulseDurationSec = 0.25f;

    /// <summary>Material to apply in the OverlayMaterial slot while a mission is running (travel/work/return).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Highlight")
    TObjectPtr<UMaterialInterface> activeMissionOverlayMaterial = nullptr;

    /// <summary>If true, restores the previous OverlayMaterial when highlight is disabled.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Highlight")
    bool bRestorePreviousOverlayMaterial = true;

    /// <summary>If true, highlights by enabling CustomDepth on all cached primitives.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Highlight")
    bool bUseCustomDepthHighlight = true;

    /// <summary>Stencil value used for outline materials (if your post process reads it).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Highlight", meta=(ClampMin="0", ClampMax="255"))
    int32 customDepthStencilValue = 1;

#pragma endregion SETTINGS

public:
#pragma region API

    /// <summary>Returns the mission location.</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|Map|Site")
    EDispatchMissionLocation GetMissionLocation() const { return missionLocation; }

    /// <summary>Rebuilds the cached primitive components list (site + target actors).</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Site")
    void RebuildMeshCache();

    /// <summary>Sets offer active highlight state.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Site")
    void SetOfferActive(bool bActive);

    /// <summary>Plays a short pulse highlight when an offer expires (no agents sent).</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Site")
    void PlayOfferExpiredPulse();

    /// <summary>Sets highlight state based on mission state.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Site")
    void SetMissionState(EDispatchMissionState NewState);

#pragma endregion API

protected:
#pragma region INTERNAL

    /// <summary>Refreshes highlight by evaluating offer/mission/pulse state and choosing the right material.</summary>
    void RefreshHighlight();

    /// <summary>Ends the offer expired pulse and restores the correct highlight state.</summary>
    void EndOfferExpiredPulse();

    /// <summary>Applies highlight to cached components (OverlayMaterial and/or CustomDepth).</summary>
    void ApplyHighlight(bool bEnabled, int32 Stencil);

#pragma endregion INTERNAL

private:
#pragma region STATE

    UPROPERTY(Transient)
    TArray<TObjectPtr<UPrimitiveComponent>> cachedPrimitives;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMeshComponent>> cachedMeshes;

    /// <summary>Cached overlay material per mesh to restore after highlight ends.</summary>
    TMap<TWeakObjectPtr<UMeshComponent>, TObjectPtr<UMaterialInterface>> previousOverlayByMesh;

    bool bOfferExpiredPulseActive = false;
    FTimerHandle offerExpiredPulseTimerHandle;

    bool bOfferActive = false;
    EDispatchMissionState currentMissionState = EDispatchMissionState::None;

#pragma endregion STATE
};