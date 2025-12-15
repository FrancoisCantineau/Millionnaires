/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchCameraManagerComponent" - Header
 * Notes: Component responsible for registering and cycling between Dispatch camera spots.
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Dispatch/DispatchTypes.h"
#include "DispatchCameraManagerComponent.generated.h"

class ADispatchCameraSpot;
class APlayerController;

/**
 * Component that keeps track of available DispatchCameraSpot instances and handles:
 * - camera cycling / activation
 * - smooth zoom (FOV + optional postprocess) on the active camera
 * - camera micro-parallax (mouse-driven yaw/pitch) and optional zoom look-at behaviour
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MILLIONNAIRES_API UDispatchCameraManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /** Default constructor. */
    UDispatchCameraManagerComponent();

#pragma region CONFIG

protected:
    /** If true, the component will scan the world for existing camera spots on BeginPlay. */
    UPROPERTY(EditAnywhere, Category = "Dispatch|Camera")
    bool bAutoDiscoverCameras = true;

    /** Enables mouse-driven micro-parallax on the active camera spot actor. */
    UPROPERTY(EditAnywhere, Category = "Dispatch|Camera|Mouse")
    bool bEnableMouseParallax = true;

    /** Maximum yaw offset in degrees when moving the mouse horizontally. */
    UPROPERTY(EditAnywhere, Category = "Dispatch|Camera|Mouse", meta = (EditCondition = "bEnableMouseParallax"))
    float MouseYawAmplitude = 8.f;

    /** Maximum pitch offset in degrees when moving the mouse vertically. */
    UPROPERTY(EditAnywhere, Category = "Dispatch|Camera|Mouse", meta = (EditCondition = "bEnableMouseParallax"))
    float MousePitchAmplitude = 4.f;

    /** Interp speed used to smoothly move the camera towards the target rotation. */
    UPROPERTY(EditAnywhere, Category = "Dispatch|Camera|Mouse")
    float CameraRotationInterpSpeed = 5.f;

    /** If true, when zooming we bias the rotation toward a world target under the cursor. */
    UPROPERTY(EditAnywhere, Category = "Dispatch|Camera|Zoom")
    bool bEnableZoomLookAt = true;

    /** Interp speed used when rotating the camera to look at the zoom target. */
    UPROPERTY(EditAnywhere, Category = "Dispatch|Camera|Zoom", meta = (EditCondition = "bEnableZoomLookAt"))
    float ZoomLookAtInterpSpeed = 4.f;

#pragma endregion CONFIG

#pragma region STATE

protected:
    /** All registered camera spots available for cycling. */
    UPROPERTY(Transient)
    TArray<TWeakObjectPtr<ADispatchCameraSpot>> CameraSpots;

    /** Index of the currently active camera within the CameraSpots array. */
    UPROPERTY(Transient)
    int32 ActiveCameraIndex = INDEX_NONE;

    /** True while the player is holding the zoom input (right mouse button). */
    UPROPERTY(Transient)
    bool bWantsZoom = false;

    /** Current zoom alpha [0..1] interpolated over time. */
    UPROPERTY(Transient)
    float CurrentZoomAlpha = 0.f;

    /** Initial FOV of the active camera at the moment it became active (stable base for lerp). */
    UPROPERTY(Transient)
    float ActiveCameraInitialFOV = 90.f;

    /** Base rotation of the active camera (before mouse offset is applied). */
    UPROPERTY(Transient)
    FRotator BaseCameraRotation = FRotator::ZeroRotator;

    /** True once base rotation has been cached for the active camera. */
    UPROPERTY(Transient)
    bool bHasBaseRotation = false;

    /** World location that the camera should look at when zooming (computed under the cursor). */
    UPROPERTY(Transient)
    FVector ZoomTargetWorldLocation = FVector::ZeroVector;

    /** True if a valid zoom target was computed. */
    UPROPERTY(Transient)
    bool bHasZoomTarget = false;

#pragma endregion STATE

#pragma region DELEGATES

public:
    /** Event fired whenever the active Dispatch camera changes. */
    UPROPERTY(BlueprintAssignable, Category = "Dispatch|Camera")
    FDispatchCameraChanged OnActiveCameraChanged;

#pragma endregion DELEGATES

#pragma region LIFECYCLE

protected:
    /** Called when the game starts. */
    virtual void BeginPlay() override;

    /** Tick used to update smooth zoom and camera behaviour on the active camera. */
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion LIFECYCLE

#pragma region REGISTRATION

public:
    /** Registers a camera spot so it can be cycled to. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera")
    void RegisterCameraSpot(ADispatchCameraSpot* CameraSpot);

    /** Unregisters a camera spot, ex: when it is destroyed. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera")
    void UnregisterCameraSpot(ADispatchCameraSpot* CameraSpot);

#pragma endregion REGISTRATION

#pragma region CAMERA_CONTROL

public:
    /** Activates the next camera to the right (increment index). */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera")
    void CycleCameraRight();

    /** Activates the next camera to the left (decrement index). */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera")
    void CycleCameraLeft();

    /** Activates a camera by index in the registered list. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera")
    void ActivateCameraByIndex(int32 Index);

    /** Returns the currently active camera spot. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera")
    ADispatchCameraSpot* GetActiveCamera() const;

#pragma endregion CAMERA_CONTROL

#pragma region ZOOM

public:
    /** Sets whether the player currently wants to zoom (pressed/held). */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera|Zoom")
    void SetWantsZoom(bool bInWantsZoom);

protected:
    /** Applies smooth zoom on the active camera each tick. */
    void UpdateZoom(float DeltaTime);

    /** Computes a world-space zoom target under the cursor for look-at bias. */
    void ComputeZoomTargetUnderCursor();

#pragma endregion ZOOM

#pragma region CAMERA_BEHAVIOUR

protected:
    /** Applies mouse parallax and optional zoom look-at bias on the active camera spot actor. */
    void UpdateActiveCameraRotation(float DeltaTime);

#pragma endregion CAMERA_BEHAVIOUR

#pragma region INTERNAL

protected:
    /** Sorts the list of cameras by their CameraIndex property. */
    void SortCameraSpots();

    /** Returns the owning player controller, if any. */
    APlayerController* GetOwningPlayerController() const;

#pragma endregion INTERNAL
};
