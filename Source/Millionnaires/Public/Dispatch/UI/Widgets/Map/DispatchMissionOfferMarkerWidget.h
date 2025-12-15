/* 
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionOfferMarkerWidget" - Header
 * Notes: Base widget for mission offer markers shown on the map.
 */
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DispatchMissionOfferMarkerWidget.generated.h"

class UButton;

USTRUCT(BlueprintType)
struct FDispatchMapOfferMarker
{
    GENERATED_BODY()

    /** Offer id. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map")
    FGuid offerId;

    /** World location for the mission. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map")
    FVector worldLocation = FVector::ZeroVector;

    /** Normalized map position [0..1] computed from map bounds. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map")
    FVector2D normalizedPos = FVector2D(0.5, 0.5);

    /** Display title. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map")
    FText title;

    /** Mission type as int (keeps UI loosely coupled). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map")
    int32 missionType = 0;

    /** Difficulty bars 0..10. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map")
    int32 monsterChance10 = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map")
    int32 lootChance10 = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map")
    int32 complicationChance10 = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchOfferMarkerClicked, const FGuid&, OfferId);

/**
 * Marker widget displayed on the map for a mission offer.
 * Blueprint defines visuals; this base provides click + selection behavior.
 */
UCLASS()
class MILLIONNAIRES_API UDispatchMissionOfferMarkerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
#pragma region API

    /** Initializes marker data (called by map widget if auto-spawning markers). */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Marker")
    void InitializeMarker(const FDispatchMapOfferMarker& InData);

    /** Sets selection state (for highlight). */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Marker")
    void SetSelected(bool bInSelected);

    /** Returns offer id. */
    UFUNCTION(BlueprintPure, Category="Dispatch|Map|Marker")
    FGuid GetOfferId() const { return data.offerId; }

#pragma endregion API

#pragma region EVENTS

    /** Fired when the marker is clicked. */
    UPROPERTY(BlueprintAssignable, Category="Dispatch|Map|Marker|Events")
    FDispatchOfferMarkerClicked OnMarkerClicked;

#pragma endregion EVENTS

protected:
#pragma region UUSERWIDGET

    virtual void NativeConstruct() override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

#pragma endregion UUSERWIDGET

#pragma region BINDINGS

    /** Optional button for clicks (recommended in BP). */
    UPROPERTY(meta=(BindWidgetOptional))
    UButton* clickButton = nullptr;

#pragma endregion BINDINGS

#pragma region STATE

    UPROPERTY(BlueprintReadOnly, Category="Dispatch|Map|Marker")
    FDispatchMapOfferMarker data;

    UPROPERTY(BlueprintReadOnly, Category="Dispatch|Map|Marker")
    bool bIsSelected = false;

#pragma endregion STATE

#pragma region BP_HOOKS

    /** Called after InitializeMarker. */
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|Map|Marker")
    void HandleMarkerDataApplied(const FDispatchMapOfferMarker& InData);

    /** Called when selection changes. */
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|Map|Marker")
    void HandleSelectedChanged(bool bSelected);

#pragma endregion BP_HOOKS

private:
#pragma region INTERNAL

    UFUNCTION()
    void HandleButtonClicked();

#pragma endregion INTERNAL
};
