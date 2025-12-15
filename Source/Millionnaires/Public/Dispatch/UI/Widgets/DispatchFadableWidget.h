/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchFadableWidget" - Header
 * Notes: Simple UUserWidget base that supports fade in/out via RequestVisible().
 */
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DispatchFadableWidget.generated.h"

/**
 * Base UMG widget with built-in fade in/out:
 * - RequestVisible(true)  -> fades to opacity 1 and becomes visible
 * - RequestVisible(false) -> fades to opacity 0 then becomes hidden
 *
 * No UMG animation setup required.
 */
UCLASS()
class MILLIONNAIRES_API UDispatchFadableWidget : public UUserWidget
{
    GENERATED_BODY()

public:
#pragma region API

    /** Requests the widget to fade in/out. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|Fade")
    void RequestVisible(bool bVisible);

    /** Returns whether visibility is currently requested. */
    UFUNCTION(BlueprintPure, Category="Dispatch|UI|Fade")
    bool IsVisibleRequested() const { return bTargetVisible; }

#pragma endregion API

protected:
#pragma region UUSERWIDGET

    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

#pragma endregion UUSERWIDGET

#pragma region SETTINGS

    /** Fade-in speed (higher = faster). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Fade", meta=(ClampMin="0.1"))
    float FadeInSpeed = 10.f;

    /** Fade-out speed (higher = faster). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Fade", meta=(ClampMin="0.1"))
    float FadeOutSpeed = 10.f;

    /** When fading out, hide the widget once opacity is below this threshold. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Fade", meta=(ClampMin="0.0", ClampMax="1.0"))
    float HideThreshold = 0.02f;

#pragma endregion SETTINGS

#pragma region STATE

    /** Target visibility. */
    bool bTargetVisible = false;

    /** Current opacity used for fade. */
    float CurrentOpacity = 0.f;

#pragma endregion STATE
};
