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
#include "DispatchTypes.h"
#include "DispatchPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UDispatchCameraManagerComponent;
class UDispatchCursorComponent;
class UDispatchCursorRadialWidget;
class UUserWidget;
class ADispatchCameraSpot;

/**
 * Player controller used while the player is in the Dispatch control room.
 * Holds the camera manager, cursor component and map UI.
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

#pragma region CAMERA_CONFIG

protected:
    /** Maximum yaw offset in degrees when moving the mouse horizontally.. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Mouse")
    float MouseYawAmplitude;

    /** Maximum pitch offset in degrees when moving the mouse vertically. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Mouse")
    float MousePitchAmplitude;

    /** Interp speed used to smoothly move the camera towards the mouse-based rotation. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Mouse")
    float CameraRotationInterpSpeed;

    /** FOV used when fully zoomed in. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Zoom")
    float ZoomedFOV;

    /** Interp speed used to reach the target FOV when zooming. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Zoom")
    float ZoomFOVInterpSpeed;

    /** Interp speed used when rotating the camera to look at the zoom target. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Zoom")
    float ZoomLookAtInterpSpeed;

    /** Duration of the fade-to-black when switching cameras. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Fade")
    float CameraFadeOutDuration;

    /** Duration of the fade-in-from-black when switching cameras. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Fade")
    float CameraFadeInDuration;

#pragma endregion CAMERA_CONFIG

#pragma region UI

protected:
    /** Widget class used for the main Dispatch map opened by the hologram. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|UI")
    TSubclassOf<UUserWidget> MapWidgetClass;

    /** Instance of the currently opened map widget, if any. */
    UPROPERTY(Transient)
    UUserWidget* ActiveMapWidget;

    /** Widget class used to display the radial cursor fill around the mouse. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|UI")
    TSubclassOf<UDispatchCursorRadialWidget> CursorRadialWidgetClass;

    /** Instance of the radial cursor widget. */
    UPROPERTY(Transient)
    UDispatchCursorRadialWidget* CursorRadialWidgetInstance;

#pragma endregion UI

#pragma region CAMERA_STATE

protected:
    /** Current active Dispatch camera spot provided by the camera manager. */
    UPROPERTY(Transient)
    TWeakObjectPtr<ADispatchCameraSpot> CurrentCameraSpot;

    /** Base rotation of the active camera (before mouse offset is applied). */
    UPROPERTY(Transient)
    FRotator BaseCameraRotation;

    /** Default FOV of the active camera before zoom is applied. */
    UPROPERTY(Transient)
    float DefaultCameraFOV;

    /** Current FOV applied to the active camera. */
    UPROPERTY(Transient)
    float CurrentCameraFOV;

    /** True once defaults (rotation/FOV) have been cached from the active camera. */
    UPROPERTY(Transient)
    bool bHasCameraDefaults;

    /** Is the player currently holding the zoom input. */
    UPROPERTY(Transient)
    bool bWantsZoom;

    /** World location that the camera should look at when zooming (under the cursor). */
    UPROPERTY(Transient)
    FVector ZoomTargetWorldLocation;

    /** True if a valid zoom target was computed. */
    UPROPERTY(Transient)
    bool bHasZoomTarget;

    /** Currently pending camera switch (left/right) while the fade is playing. */
    UPROPERTY(Transient)
    EDispatchPendingCameraSwitch PendingCameraSwitch = EDispatchPendingCameraSwitch::None;

    /** Optional override for the camera actor to switch to after fade. */
    UPROPERTY(Transient)
    TWeakObjectPtr<ADispatchCameraSpot> PendingCameraOverride;

    /** True while a fade sequence (out+switch+in) is running. */
    UPROPERTY(Transient)
    bool bIsCameraFading;

    /** TimerHandle used to trigger camera switch and fade-in. */
    FTimerHandle CameraFadeTimerHandle;

#pragma endregion CAMERA_STATE

#pragma region LIFECYCLE

protected:
    /** Called when the game starts. */
    virtual void BeginPlay() override;

    /** Called to bind functionality to input. */
    virtual void SetupInputComponent() override;

    /** Per-frame update for the player controller. */
    virtual void PlayerTick(float DeltaTime) override;

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

#pragma region MAP_API

public:
    /** Opens the Dispatch map widget if it is not already open. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|UI")
    void OpenMap();

    /** Closes the Dispatch map widget if it is currently open. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|UI")
    void CloseMap();

    /** Toggles the Dispatch map widget on/off. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|UI")
    void ToggleMap();

#pragma endregion MAP_API

#pragma region INTERNAL_CALLBACKS

protected:
    /** Called whenever the active Dispatch camera changes, used to cache defaults and auto-close the map. */
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

    /** Internal: updates camera rotation/FOV according to mouse and zoom each frame. */
    void UpdateCameraMouseAndZoom(float DeltaTime);

#pragma endregion INTERNAL_CALLBACKS
};
