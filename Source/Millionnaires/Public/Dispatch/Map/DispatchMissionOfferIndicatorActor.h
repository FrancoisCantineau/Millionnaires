/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionOfferIndicatorActor" - Header
 * Notes: World-space mission offer indicator (WidgetComponent) spawned on NotificationPoints.
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dispatch/Missions/DispatchMissionTypes.h"
#include "DispatchMissionOfferIndicatorActor.generated.h"

class UWidgetComponent;
class UDispatchMissionManagerComponent;
class UDispatchMissionWorldNotificationWidget;
class UDispatchMissionOfferClickProxyComponent;

/**
 * World-space actor spawned for each mission offer.
 * - Holds a WidgetComponent (WBP based on UDispatchMissionWorldNotificationWidget).
 * - Reads offer time remaining from UDispatchMissionManagerComponent and updates widget progress.
 * - Can be clicked (through DispatchCursorComponent) to open mission details via a ClickProxy.
 */
UCLASS()
class MILLIONNAIRES_API ADispatchMissionOfferIndicatorActor : public AActor
{
    GENERATED_BODY()

public:
#pragma region LIFECYCLE

    /// <summary>Constructor.</summary>
    ADispatchMissionOfferIndicatorActor();

    /// <summary>Tick: updates widget progress.</summary>
    virtual void Tick(float DeltaSeconds) override;

#pragma endregion LIFECYCLE

public:
#pragma region SETTINGS

    /// <summary>Widget component showing the world notification.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Indicator")
    TObjectPtr<UWidgetComponent> widgetComponent;

    /// <summary>If true, this actor updates its widget every tick.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Indicator")
    bool bTickUpdateWidget = true;

    /// <summary>Optional - allow auto destroy if the offer no longer exists.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Indicator")
    bool bAutoDestroyIfOfferMissing = true;

    /// <summary>If true, keeps the widget facing the player's camera while staying in World space.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Indicator")
    bool bBillboardToCamera = true;

    /// <summary>If true, billboard uses yaw only (keeps the widget upright).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Indicator")
    bool bBillboardYawOnly = true;

    /// <summary>Optional update interval for billboard updates (0 = every tick).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Indicator", meta=(ClampMin="0.0"))
    float billboardUpdateInterval = 0.f;

#pragma endregion SETTINGS

public:
#pragma region API

    /// <summary>Initializes the indicator with mission manager + offer id.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Indicator")
    void Initialize(UDispatchMissionManagerComponent* InMissionManager, const FGuid& InOfferId);

    /// <summary>Returns the offer id (valid after Initialize).</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|Map|Indicator")
    FGuid GetOfferId() const { return offerId; }

#pragma endregion API

protected:
#pragma region INTERNAL

    /// <summary>Updates widget values from the offer.</summary>
    void UpdateWidgetFromOffer();

    /// <summary>Returns the typed widget if present.</summary>
    UDispatchMissionWorldNotificationWidget* GetTypedWidget() const;

#pragma endregion INTERNAL

private:
#pragma region STATE

    TWeakObjectPtr<UDispatchMissionManagerComponent> missionManager;

    UPROPERTY(VisibleAnywhere, Category="Dispatch|Map|Indicator")
    FGuid offerId;

    UPROPERTY(Transient)
    TObjectPtr<UDispatchMissionOfferClickProxyComponent> clickProxy = nullptr;

    /// <summary>Internal billboard timer.</summary>
    float billboardAccum = 0.f;

#pragma endregion STATE
};
