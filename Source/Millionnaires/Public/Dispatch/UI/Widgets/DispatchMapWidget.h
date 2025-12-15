/* 
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMapWidget" - Header
 * Notes: Interactive map widget that displays mission offers as markers and supports pan/zoom.
 */
#pragma once

#include "CoreMinimal.h"
#include "Dispatch/UI/Widgets/DispatchFadableWidget.h"
#include "Dispatch/UI/Widgets/Map/DispatchMissionOfferMarkerWidget.h"
#include "Dispatch/UI/Widgets/Map/DispatchMissionOfferDetailsWidget.h"
#include "DispatchMapWidget.generated.h"

class UCanvasPanel;
class UPanelWidget;
class UDispatchMissionManagerComponent;
class ADispatchMapBoundsVolume;

#pragma region DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDispatchMapCloseRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchMapOfferSelected, const FGuid&, OfferId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchMapOfferAcceptRequested, const FGuid&, OfferId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchMapOfferDeclineRequested, const FGuid&, OfferId);

#pragma endregion DELEGATES

/**
 * Map widget responsibilities:
 * - Displays mission offers (from UDispatchMissionManagerComponent) as markers on a 2D map
 * - Converts world locations to normalized map coordinates using ADispatchMapBoundsVolume
 * - Supports pan/zoom with mouse wheel and RMB drag
 * - Emits selection + accept/decline requests (agent selection is handled elsewhere)
 *
 * Visuals are defined in Blueprint:
 * - mapContent (optional) -> widget to apply pan/zoom render transform
 * - markersCanvas (optional) -> canvas where markers are auto-positioned
 * - detailsHost (optional) -> panel that hosts the details widget instance
 */
UCLASS()
class MILLIONNAIRES_API UDispatchMapWidget : public UDispatchFadableWidget
{
    GENERATED_BODY()

public:
#pragma region API_SETUP

    /** Sets mission manager source and binds to offer events. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map")
    void SetMissionManager(UDispatchMissionManagerComponent* InMissionManager);

    /** Sets the bounds actor used for world->map conversion (optional). If null, auto-find is used. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map")
    void SetMapBoundsVolume(ADispatchMapBoundsVolume* InBoundsVolume);

    /** Call this from Blueprint (close button) to request closing the map. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map")
    void RequestClose();

#pragma endregion API_SETUP

#pragma region API_OFFERS

    /** Forces a full refresh of offers and markers. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Offers")
    void RefreshOffers();

    /** Selects an offer by id (updates details + marker selection). */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Offers")
    void SelectOffer(const FGuid& OfferId);

    /** Clears current selection. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Offers")
    void ClearSelection();

    /** Returns the currently selected offer id (invalid if none). */
    UFUNCTION(BlueprintPure, Category="Dispatch|Map|Offers")
    FGuid GetSelectedOfferId() const { return selectedOfferId; }

#pragma endregion API_OFFERS

#pragma region API_TRANSFORM

    /** Adds zoom (mouse wheel). Positive zooms in. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Transform")
    void AddZoom(float Delta);

    /** Adds pan translation (drag). */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Transform")
    void AddPan(const FVector2D& Delta);

    /** Resets pan/zoom. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Transform")
    void ResetTransform();

#pragma endregion API_TRANSFORM

#pragma region EVENTS

    /** Close requested by the map UI (e.g., close button). */
    UPROPERTY(BlueprintAssignable, Category="Dispatch|Map|Events")
    FDispatchMapCloseRequested OnCloseRequested;


    /** Selected offer changed. */
    UPROPERTY(BlueprintAssignable, Category="Dispatch|Map|Events")
    FDispatchMapOfferSelected OnOfferSelected;

    /** Player pressed accept in details panel. */
    UPROPERTY(BlueprintAssignable, Category="Dispatch|Map|Events")
    FDispatchMapOfferAcceptRequested OnOfferAcceptRequested;

    /** Player declined an offer. */
    UPROPERTY(BlueprintAssignable, Category="Dispatch|Map|Events")
    FDispatchMapOfferDeclineRequested OnOfferDeclineRequested;

#pragma endregion EVENTS

#pragma region SETTINGS

    /** If true, auto-spawns marker widgets into markersCanvas. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Markers")
    bool bAutoSpawnMarkers = true;

    /** Marker widget class spawned for each offer (optional). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Markers")
    TSubclassOf<UDispatchMissionOfferMarkerWidget> markerWidgetClass;

    /** Details widget class spawned into detailsHost when selecting an offer (optional). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Details")
    TSubclassOf<UDispatchMissionOfferDetailsWidget> detailsWidgetClass;

    /** If true, inverts Y when mapping world->UI (depends on your map orientation). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Mapping")
    bool bInvertY = true;

    /** Minimum zoom scale. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Transform", meta=(ClampMin="0.2"))
    float minZoom = 0.6f;

    /** Maximum zoom scale. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Transform", meta=(ClampMin="0.2"))
    float maxZoom = 2.4f;

    /** Mouse wheel zoom speed. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Transform", meta=(ClampMin="0.01"))
    float zoomStep = 0.15f;

#pragma endregion SETTINGS

protected:
#pragma region UUSERWIDGET

    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

#pragma endregion UUSERWIDGET

#pragma region BINDINGS

    /** Optional: widget that receives render transform for pan/zoom. (Name it 'MapContent' in BP) */
    UPROPERTY(meta=(BindWidgetOptional))
    UWidget* mapContent = nullptr;

    /** Optional: canvas panel used to place markers. (Name it 'MarkersCanvas' in BP) */
    UPROPERTY(meta=(BindWidgetOptional))
    UCanvasPanel* markersCanvas = nullptr;

    /** Optional: panel used to host details widget. (Name it 'DetailsHost' in BP) */
    UPROPERTY(meta=(BindWidgetOptional))
    UPanelWidget* detailsHost = nullptr;

#pragma endregion BINDINGS

#pragma region BP_HOOKS

    /** Called after markers are rebuilt (useful if you don't want auto-spawn). */
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|Map")
    void HandleMarkersUpdated(const TArray<FDispatchMapOfferMarker>& Markers);

    /** Called when transform changes (only used if mapContent is not bound). */
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|Map")
    void HandleMapTransformChanged(float Zoom, FVector2D Pan);

#pragma endregion BP_HOOKS

#pragma region INTERNAL

    void AutoFindBoundsVolume();
    void RebuildMarkerData();
    void SpawnMarkerWidgets();
    void UpdateMarkerWidgetPositions();
    void ApplyTransform();
    FVector2D WorldToNormalized(const FVector& WorldLocation) const;
    void UpdateDetailsWidget();

#pragma endregion INTERNAL

#pragma region CALLBACKS

    UFUNCTION()
    void HandleOfferAdded(const FGuid& OfferId);

    UFUNCTION()
    void HandleOfferRemoved(const FGuid& OfferId);

    UFUNCTION()
    void HandleMarkerClicked(const FGuid& OfferId);

    UFUNCTION()
    void HandleDetailsAccept(const FGuid& OfferId);

    UFUNCTION()
    void HandleDetailsDecline(const FGuid& OfferId);

#pragma endregion CALLBACKS

private:
#pragma region STATE

    TWeakObjectPtr<UDispatchMissionManagerComponent> missionManager;
    TWeakObjectPtr<ADispatchMapBoundsVolume> boundsVolume;

    UPROPERTY(Transient)
    TArray<FDispatchMapOfferMarker> markers;

    UPROPERTY(Transient)
    TMap<FGuid, UDispatchMissionOfferMarkerWidget*> markerWidgetsByOfferId;

    UPROPERTY(Transient)
    UDispatchMissionOfferDetailsWidget* detailsWidget = nullptr;

    FGuid selectedOfferId;

    float zoom = 1.f;
    FVector2D pan = FVector2D::ZeroVector;

    bool bIsPanning = false;
    FVector2D lastMouse = FVector2D::ZeroVector;

    bool bNeedsMarkerReposition = false;

#pragma endregion STATE
};
