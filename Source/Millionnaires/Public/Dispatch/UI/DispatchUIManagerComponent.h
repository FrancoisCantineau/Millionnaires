/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchUIManagerComponent" - Header
 * Notes: Central UI owner for Dispatch mode (radial cursor, map, mission notifications later).
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DispatchUIManagerComponent.generated.h"

class UDispatchCursorComponent;
class UDispatchCursorRadialWidget;
class UDispatchMissionManagerComponent;
class UUserWidget;
class UDispatchMapWidget;
class UDispatchCameraManagerComponent;
class APawn;

#pragma region DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchMapVisibilityChanged, bool, bIsOpen);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchOfferAcceptRequestedUI, const FGuid&, OfferId);

#pragma endregion DELEGATES

/**
 * UI manager component for Dispatch mode.
 * - Owns widgets (radial cursor, map, etc.)
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

    UDispatchUIManagerComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma endregion LIFECYCLE

#pragma region API_MAP

    /** Opens the map widget if configured. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|Map")
    void OpenMap();

    /** Closes the map widget if present. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|Map")
    void CloseMap();

    /** Toggles map widget visibility. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|Map")
    void ToggleMap();

    /** Returns whether map is open. */
    UFUNCTION(BlueprintPure, Category="Dispatch|UI|Map")
    bool IsMapOpen() const { return bIsMapOpen; }

#pragma endregion API_MAP

#pragma region API_MISSIONS

    /**
     * Attempts to accept a mission offer using the selected agents.
     * UI should pick agents (multi for Dispatch mission, single for FPS mission later) then call this.
     */
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|Missions")
    bool TryAcceptOffer(const FGuid& OfferId, const TArray<APawn*>& SelectedAgents);

#pragma endregion API_MISSIONS


#pragma region EVENTS

    /** Fired when the map open state changes. */
    UPROPERTY(BlueprintAssignable, Category="Dispatch|UI|Events")
    FDispatchMapVisibilityChanged OnMapVisibilityChanged;

    /** Map requested to accept an offer. UI must pick agents then call TryAcceptOffer(). */
    UPROPERTY(BlueprintAssignable, Category="Dispatch|UI|Events")
    FDispatchOfferAcceptRequestedUI OnOfferAcceptRequested;

#pragma endregion EVENTS

#pragma region SETTINGS

    /** Cursor radial widget class (WBP based on UDispatchCursorRadialWidget). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Cursor")
    TSubclassOf<UDispatchCursorRadialWidget> cursorRadialWidgetClass;

    /** Radial ZOrder. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Cursor")
    int32 cursorRadialZOrder = 999;

    /** Map widget class (optional). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Map")
    TSubclassOf<UDispatchMapWidget> mapWidgetClass;

    /** Map ZOrder. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Map")
    int32 mapZOrder = 50;

    /** Actor tag: clicking this toggles the map (hologram). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Map")
    FName mapHologramTag = FName("DispatchMapHologram");

#pragma endregion SETTINGS

protected:
#pragma region INTERNAL_BINDINGS

    /** Binds to Cursor and Mission manager components. */
    void BindToComponents();

    /** Unbinds from Cursor and Mission manager components. */
    void UnbindFromComponents();

#pragma endregion INTERNAL_BINDINGS

#pragma region INTERNAL_WIDGETS

    /** Creates the radial widget and adds it to viewport. */
    void CreateCursorRadialWidget();

    /** Ensures map widget exists and sets its visibility. */
    void SetMapWidgetVisible(bool bVisible);

#pragma endregion INTERNAL_WIDGETS

#pragma region INTERNAL_CALLBACKS

    /** Cursor hover UI progress callback. */
    UFUNCTION()
    void HandleHoverUIProgress(float Progress, FVector2D ScreenPos, bool bVisible);

    /** Cursor click callback. */
    UFUNCTION()
    void HandleActorClicked(AActor* ClickedActor);

    /** Mission offer added callback (placeholder for notifications). */
    UFUNCTION()
    void HandleOfferAdded(const FGuid& OfferId);

    /** Map requested decline -> forward to mission manager. */
    UFUNCTION()
    void HandleOfferDeclinedFromMap(const FGuid& OfferId);

    /** Map requested accept -> broadcast for UI selection. */
    UFUNCTION()
    void HandleOfferAcceptedFromMap(const FGuid& OfferId);

#pragma endregion INTERNAL_CALLBACKS

private:
#pragma region STATE

    TWeakObjectPtr<UDispatchCursorComponent> cursorComponent;
    TWeakObjectPtr<UDispatchMissionManagerComponent> missionManager;
    TWeakObjectPtr<UDispatchCameraManagerComponent> cameraManager;

    UPROPERTY(Transient)
    UDispatchCursorRadialWidget* cursorRadialWidget = nullptr;

    UPROPERTY(Transient)
    UDispatchMapWidget* mapWidget = nullptr;

    bool bIsMapOpen = false;

    /** Cached previous cursor visibility, restored on close. */
    bool bPrevShowMouseCursor = true;

#pragma endregion STATE
};
