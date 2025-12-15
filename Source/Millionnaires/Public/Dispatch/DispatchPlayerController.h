/**
 * Millionnaires Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchPlayerController"
 * Notes: PlayerController specialised for the Dispatch (control room) gameplay.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Dispatch/DispatchTypes.h"
#include "DispatchPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UDispatchCameraManagerComponent;
class UDispatchCursorComponent;
class UDispatchCursorRadialWidget;
class ADispatchCameraSpot;

/**
 * Player controller used while the player is in the Dispatch control room.
 * - Owns input bindings (Enhanced Input)
 * - Owns the camera manager + cursor component
 * - Owns camera switching fade (via PlayerCameraManager)
 *
 */
UCLASS()
class MILLIONNAIRES_API ADispatchPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    /** Default constructor. */
    ADispatchPlayerController();

#pragma region COMPONENTS

protected:
    /** Component managing all Dispatch cameras and cycling between them. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dispatch", meta = (AllowPrivateAccess = "true"))
    UDispatchCameraManagerComponent* CameraManagerComponent;

    /** Component handling cursor traces, hover logic and time dilation. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dispatch", meta = (AllowPrivateAccess = "true"))
    UDispatchCursorComponent* CursorComponent;

#pragma endregion COMPONENTS

#pragma region INPUT

protected:
    /** Mapping context used for all Dispatch input (camera, zoom, click). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dispatch|Input")
    TObjectPtr<UInputMappingContext> DispatchIMC;

    /** InputAction used to cycle camera to the left. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dispatch|Input")
    TObjectPtr<UInputAction> CameraLeftAction;

    /** InputAction used to cycle camera to the right. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dispatch|Input")
    TObjectPtr<UInputAction> CameraRightAction;

    /** InputAction used for zoom (pressed = zoom in, released = zoom out). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dispatch|Input")
    TObjectPtr<UInputAction> ZoomAction;

    /** InputAction used for clicking on interactables with the cursor. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dispatch|Input")
    TObjectPtr<UInputAction> ClickAction;

#pragma endregion INPUT

#pragma region UI

protected:
    /** Widget class used to display the radial cursor fill around the mouse. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|UI")
    TSubclassOf<UDispatchCursorRadialWidget> CursorRadialWidgetClass;

    /** Instance of the radial cursor widget. */
    UPROPERTY(Transient)
    UDispatchCursorRadialWidget* CursorRadialWidgetInstance;

#pragma endregion UI

#pragma region CAMERA_SWITCH_STATE

protected:
    /** Cached active Dispatch camera spot for convenience / debug. */
    UPROPERTY(Transient)
    TWeakObjectPtr<ADispatchCameraSpot> CurrentCameraSpot;

    /** Currently pending camera switch (left/right) while the fade is playing. */
    UPROPERTY(Transient)
    EDispatchPendingCameraSwitch PendingCameraSwitch = EDispatchPendingCameraSwitch::None;

    /** True while a fade sequence (out+switch+in) is running. */
    UPROPERTY(Transient)
    bool bIsCameraFading = false;

    /** TimerHandle used to trigger camera switch and fade-in. */
    FTimerHandle CameraFadeTimerHandle;

    /** Duration of the fade-to-black when switching cameras. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Fade")
    float CameraFadeOutDuration = 0.35f;

    /** Duration of the fade-in-from-black when switching cameras. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Fade")
    float CameraFadeInDuration = 0.35f;

#pragma endregion CAMERA_SWITCH_STATE

#pragma region LIFECYCLE

protected:
    /** Called when the game starts. */
    virtual void BeginPlay() override;

    /** Called to bind functionality to input. */
    virtual void SetupInputComponent() override;

#pragma endregion LIFECYCLE

#pragma region CAMERA_API

public:
    /** Goes to the previous Dispatch camera (cycle left). */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera")
    void CycleCameraLeft();

    /** Goes to the next Dispatch camera (cycle right). */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera")
    void CycleCameraRight();

    /** Returns the Dispatch camera manager component. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera")
    UDispatchCameraManagerComponent* GetCameraManagerComponent() const { return CameraManagerComponent; }

    /** Called when the zoom input is pressed (e.g. right mouse button down). */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera")
    void HandleZoomPressed();

    /** Called when the zoom input is released. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera")
    void HandleZoomReleased();

#pragma endregion CAMERA_API

#pragma region CURSOR_API

public:
    /** Returns the Dispatch cursor component. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Cursor")
    UDispatchCursorComponent* GetCursorComponent() const { return CursorComponent; }

    /** Called by input to click on the current interactable object. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Interaction")
    void HandleCursorClick();

#pragma endregion CURSOR_API

#pragma region INTERNAL_CALLBACKS

protected:
    /** Called whenever the active Dispatch camera changes. */
    UFUNCTION()
    void HandleActiveCameraChanged(ADispatchCameraSpot* NewCamera);

    /** Callback fired by the CursorComponent when hover progress is updated. */
    UFUNCTION()
    void HandleHoverProgress(float Progress, bool bIsHoveringCharacter);

    /** Internal: starts a fade sequence before switching camera. */
    void StartCameraFadeSequence(EDispatchPendingCameraSwitch SwitchDirection);

    /** Internal: called when the fade-to-black finishes, performs the camera switch and starts fade-in. */
    UFUNCTION()
    void HandleCameraFadeOutFinished();

    /** Internal: called when fade-in back from black has finished. */
    UFUNCTION()
    void HandleCameraFadeInFinished();

#pragma endregion INTERNAL_CALLBACKS
};
