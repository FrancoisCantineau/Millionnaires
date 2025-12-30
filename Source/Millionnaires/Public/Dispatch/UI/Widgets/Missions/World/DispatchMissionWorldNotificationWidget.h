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
 * Create a WBP inheriting from this class and bind your UI elements to the exposed variables:
 * - progress01 (0..1)
 * - titleText
 * - iconTexture
 * - timeRemainingSec / timeLimitSec
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

#pragma endregion DATA_BINDING

public:
#pragma region API

    /// <summary>Sets this widget from an offer. Calls BP_OnOfferUpdated().</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|WorldNotification")
    void SetFromOffer(const FDispatchMissionOffer& Offer);

#pragma endregion API

protected:
#pragma region BP_EVENTS

    /// <summary>Called when any offer UI value is updated (implement in WBP).</summary>
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|UI|WorldNotification")
    void BP_OnOfferUpdated();

#pragma endregion BP_EVENTS
};
