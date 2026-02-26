/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchUIManagerComponent" - Header
 * Notes: Central UI owner for Dispatch mode (radial cursor, map camera view, mission details menu).
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DispatchUIManagerComponent.generated.h"

class UDispatchCursorComponent;
class UDispatchCursorRadialWidget;
class UDispatchMissionManagerComponent;
class UDispatchMissionDetailsWidget;
class UDispatchMissionOfferClickProxyComponent;
class AActor;
class APawn;
class UDispatchCameraManagerComponent;

#pragma region DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchMapVisibilityChanged, bool, bIsOpen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchOfferAcceptRequestedUI, const FGuid&, OfferId);

#pragma endregion DELEGATES

/**
 * UI manager component for Dispatch mode.
 * - Owns widgets (radial cursor, mission details)
 * - Map is a camera view (aerial/isometric)
 * - Binds to gameplay components (cursor, mission manager) and updates UI accordingly
 *
 * PlayerController should not call widgets directly.
 */
UCLASS(ClassGroup=(Dispatch), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UDispatchUIManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
#pragma region LIFECYCLE

    /// <summary>Constructor.</summary>
    UDispatchUIManagerComponent();

    /// <summary>BeginPlay: binds to cursor/mission manager and creates widgets.</summary>
    virtual void BeginPlay() override;

    /// <summary>EndPlay: unbinds and destroys widgets.</summary>
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma endregion LIFECYCLE

#pragma region API_MAP

    /// <summary>Opens the map view (camera) if configured.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|Map")
    void OpenMap();

    /// <summary>Closes the map view (camera).</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|Map")
    void CloseMap();

    /// <summary>Toggles map view.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|Map")
    void ToggleMap();

    /// <summary>Returns whether map is open.</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|UI|Map")
    bool IsMapOpen() const { return bIsMapOpen; }

#pragma endregion API_MAP

#pragma region API_MISSION_DETAILS

    /// <summary>Opens mission details for an offer.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|Missions")
    void OpenMissionDetailsForOffer(const FGuid& OfferId);

    /// <summary>Opens mission details for an active mission.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|Missions")
    void OpenMissionDetailsForMission(const FGuid& MissionId);

    /// <summary>Closes mission details.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|Missions")
    void CloseMissionDetails();

    /// <summary>Returns whether mission details are open.</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|UI|Missions")
    bool IsMissionDetailsOpen() const { return bIsMissionDetailsOpen; }

#pragma endregion API_MISSION_DETAILS

#pragma region EVENTS

    /// <summary>Fired when the map open state changes.</summary>
    UPROPERTY(BlueprintAssignable, Category="Dispatch|UI|Events")
    FDispatchMapVisibilityChanged OnMapVisibilityChanged;

    /// <summary>Map requested to accept an offer. UI must pick agents then call AcceptOffer().</summary>
    UPROPERTY(BlueprintAssignable, Category="Dispatch|UI|Events")
    FDispatchOfferAcceptRequestedUI OnOfferAcceptRequested;

#pragma endregion EVENTS

#pragma region SETTINGS

    /// <summary>Cursor radial widget class (WBP based on UDispatchCursorRadialWidget).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Cursor")
    TSubclassOf<UDispatchCursorRadialWidget> cursorRadialWidgetClass;

    /// <summary>Radial ZOrder.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Cursor")
    int32 cursorRadialZOrder = 999;

    /// <summary>If true, opening the map switches view to an aerial/isometric camera actor instead of a widget.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Map")
    bool bUseMapCameraView = true;

    /// <summary>Tag used to find the map camera actor in the level (e.g., a CameraActor).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Map")
    FName mapViewActorTag = FName("DispatchMapView");

    /// <summary>Blend time when switching to/from the map camera view.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Map", meta=(ClampMin="0.0"))
    float mapViewBlendTime = 0.35f;

    /// <summary>Map ZOrder (unused when using camera-only).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Map")
    int32 mapZOrder = 50;

    /// <summary>Actor tag: clicking this toggles the map (hologram).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Map")
    FName mapHologramTag = FName("DispatchMapHologram");

    /// <summary>Mission details widget class (WBP based on UDispatchMissionDetailsWidget).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Missions")
    TSubclassOf<UDispatchMissionDetailsWidget> missionDetailsWidgetClass;

    /// <summary>Mission details ZOrder.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Missions")
    int32 missionDetailsZOrder = 300;

    /// <summary>If true, mission time is paused while the details menu is open.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Missions")
    bool bPauseMissionTimeWhenDetailsOpen = true;

    /// <summary>If true, only pawns with dispatchAgentTag are shown in the mission agent selection list.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Missions|Agents")
    bool bRequireDispatchAgentTag = true;

    /// <summary>Tag used to identify selectable dispatch agents in the level.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Missions|Agents")
    FName dispatchAgentTag = FName("DispatchAgent");

    /// <summary>If true, busy agents are still displayed but marked as busy (not selectable).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Missions|Agents")
    bool bShowBusyAgents = true;

#pragma endregion SETTINGS

protected:
#pragma region INTERNAL_BINDINGS

    /// <summary>Binds to Cursor and Mission manager components.</summary>
    void BindToComponents();

    /// <summary>Unbinds from Cursor and Mission manager components.</summary>
    void UnbindFromComponents();

#pragma endregion INTERNAL_BINDINGS

#pragma region INTERNAL_WIDGETS

    /// <summary>Creates the radial widget and adds it to viewport.</summary>
    void CreateCursorRadialWidget();

    /// <summary>Creates the mission details widget (hidden by default).</summary>
    void EnsureMissionDetailsWidget();

#pragma endregion INTERNAL_WIDGETS

#pragma region INTERNAL_CALLBACKS

    /// <summary>Cursor hover UI progress callback.</summary>
    UFUNCTION()
    void HandleHoverUIProgress(float Progress, FVector2D ScreenPos, bool bVisible);

    /// <summary>Cursor click callback.</summary>
    UFUNCTION()
    void HandleActorClicked(AActor* ClickedActor);

    /// <summary>Mission offer added callback (debug).</summary>
    UFUNCTION()
    void HandleOfferAdded(const FGuid& OfferId);

    /// <summary>Map requested decline -> forward to mission manager.</summary>
    UFUNCTION()
    void HandleOfferDeclinedFromMap(const FGuid& OfferId);

    /// <summary>Map requested accept -> broadcast for UI selection.</summary>
    UFUNCTION()
    void HandleOfferAcceptedFromMap(const FGuid& OfferId);

    /// <summary>Details widget close callback.</summary>
    UFUNCTION()
    void HandleDetailsCloseRequested();

    
    /// <summary>Details widget accept callback.</summary>
    UFUNCTION()
    void HandleDetailsAcceptRequested(const FGuid& OfferId, const TArray<APawn*>& SelectedAgents);

    /// <summary>Gather selectable agents in the world.</summary>
    void GatherSelectableAgents(TArray<APawn*>& OutAgents) const;

#pragma endregion INTERNAL_CALLBACKS

private:
#pragma region STATE

    TWeakObjectPtr<UDispatchCursorComponent> cursorComponent;
    TWeakObjectPtr<UDispatchMissionManagerComponent> missionManager;
    TWeakObjectPtr<UDispatchCameraManagerComponent> cameraManager;

    UPROPERTY(Transient)
    UDispatchCursorRadialWidget* cursorRadialWidget = nullptr;

    UPROPERTY(Transient)
    UDispatchMissionDetailsWidget* missionDetailsWidget = nullptr;

    bool bIsMapOpen = false;
    bool bIsMissionDetailsOpen = false;

    /// <summary>Cached previous cursor visibility, restored on close.</summary>
    bool bPrevShowMouseCursor = true;

    /// <summary>Cached previous view target when entering map camera view.</summary>
    TWeakObjectPtr<AActor> previousViewTarget;

    /// <summary>Cached map view actor.</summary>
    TWeakObjectPtr<AActor> mapViewActor;

#pragma endregion STATE
};
