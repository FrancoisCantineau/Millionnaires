/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionSiteActor" - Header
 * Notes: Represents a mission site in the Dispatch map and supports highlight + click targets.
 */
#pragma once

#include "CoreMinimal.h"
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

    /// <summary>Sets highlight state based on mission state.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Site")
    void SetMissionState(EDispatchMissionState NewState);

#pragma endregion API

protected:
#pragma region INTERNAL

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

    bool bOfferActive = false;
    EDispatchMissionState currentMissionState = EDispatchMissionState::None;

#pragma endregion STATE
};
