/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionWorldNotificationWidget" - Header
 * Notes: Base widget for world-space mission notifications (radial timer + icon + title).
 */
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dispatch/Missions/DispatchMissionTypes.h"
#include "DispatchMissionWorldNotificationWidget.generated.h"

class UTexture2D;

/**
 * Base widget for world notifications.
 * Create a WBP inheriting from this class and update your UI from the BP events.
 * This avoids hard dependencies on specific UMG controls (e.g. RadialSlider) in C++.
 */
UCLASS(Abstract, BlueprintType)
class MILLIONNAIRES_API UDispatchMissionWorldNotificationWidget : public UUserWidget
{
    GENERATED_BODY()

public:
#pragma region DATA_BINDING

    /// <summary>Progress 0..1 (1 = full time left, 0 = expired).</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|WorldNotification")
    float progress01 = 1.f;

    /// <summary>Offer title.</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|WorldNotification")
    FText titleText;

    /// <summary>Offer icon.</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|WorldNotification")
    TObjectPtr<UTexture2D> iconTexture = nullptr;

    /// <summary>Seconds remaining.</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|WorldNotification")
    float timeRemainingSec = 0.f;

    /// <summary>Total time limit seconds.</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|WorldNotification")
    float timeLimitSec = 0.f;

    /// <summary>If true, radial fill is inverted (value = 1 - progress01).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|WorldNotification")
    bool bInvertRadialFill = false;

#pragma endregion DATA_BINDING

public:
#pragma region API

    /// <summary>
    /// Sets this widget from an offer and notifies Blueprint.
    /// You should implement BP_OnProgress01Changed / BP_OnIconChanged / BP_OnTitleChanged in your WBP.
    /// </summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|WorldNotification")
    void SetFromOffer(const FDispatchMissionOffer& Offer);

#pragma endregion API

protected:
#pragma region BP_EVENTS

    /// <summary>Called after progress01 is updated (Display01 already accounts for bInvertRadialFill).</summary>
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|UI|WorldNotification")
    void BP_OnProgress01Changed(float Display01);

    /// <summary>Called after iconTexture is updated.</summary>
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|UI|WorldNotification")
    void BP_OnIconChanged(UTexture2D* NewIcon);

    /// <summary>Called after titleText is updated.</summary>
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|UI|WorldNotification")
    void BP_OnTitleChanged(const FText& NewTitle);

    /// <summary>Called after timeRemainingSec / timeLimitSec are updated.</summary>
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|UI|WorldNotification")
    void BP_OnTimeChanged(float RemainingSec, float LimitSec);

    /// <summary>Optional: called once after all UI values were updated.</summary>
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|UI|WorldNotification")
    void BP_OnOfferUpdated();

#pragma endregion BP_EVENTS
};
