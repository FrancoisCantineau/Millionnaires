/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchCursorRadialWidget" - Header
 * Notes: Base widget for cursor radial (progress + screen position + fade in/out).
 */
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DispatchCursorRadialWidget.generated.h"

/**
 * Base widget used by Dispatch UI to display a radial progress at the mouse position.
 * - SetProgress() drives the visual (Blueprint implements HandleProgressChanged)
 * - SetScreenPosition() moves the widget to the cursor
 * - RequestVisible() fades the widget in/out (no UMG animations required)
 */
UCLASS()
class MILLIONNAIRES_API UDispatchCursorRadialWidget : public UUserWidget
{
    GENERATED_BODY()

public:
#pragma region API

    /** Sets the current progress in [0,1] and notifies Blueprints. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Cursor")
    void SetProgress(float InProgress);

    /** Moves the widget so that it is centered on the given screen position. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Cursor")
    void SetScreenPosition(const FVector2D& InScreenPosition);

    /** Requests the widget to fade in/out. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Cursor")
    void RequestVisible(bool bVisible);

#pragma endregion API

protected:
#pragma region UUSERWIDGET

    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

#pragma endregion UUSERWIDGET

#pragma region STATE

    /** Current radial progress (0 = empty, 1 = full). */
    UPROPERTY(BlueprintReadOnly, Category = "Dispatch|Cursor")
    float CurrentProgress = 0.f;

    /** Whether we want the widget visible (faded in). */
    bool bTargetVisible = false;

    /** Current render opacity used for fade. */
    float CurrentOpacity = 0.f;

#pragma endregion STATE

#pragma region FADE_SETTINGS

    /** Fade-in speed (higher = faster). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Cursor|Fade", meta=(ClampMin="0.1"))
    float FadeInSpeed = 18.f;

    /** Fade-out speed (higher = faster). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Cursor|Fade", meta=(ClampMin="0.1"))
    float FadeOutSpeed = 14.f;

    /** When fading out, hide the widget once opacity is below this threshold. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Cursor|Fade", meta=(ClampMin="0.0", ClampMax="1.0"))
    float HideThreshold = 0.02f;

#pragma endregion FADE_SETTINGS

#pragma region BP_HOOKS

    /** Implement this in Blueprint to drive your actual radial slider/material. */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dispatch|Cursor")
    void HandleProgressChanged(float InProgress);

#pragma endregion BP_HOOKS
};
