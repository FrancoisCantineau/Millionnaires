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
 * Component that keeps track of available DispatchCameraSpot instances and handles camera cycling.
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

#pragma endregion CONFIG

#pragma region STATE

protected:
    /** All registered camera spots available for cycling. */
    UPROPERTY(Transient)
    TArray<TWeakObjectPtr<ADispatchCameraSpot>> CameraSpots;

    /** Index of the currently active camera within the CameraSpots array. */
    UPROPERTY(Transient)
    int32 ActiveCameraIndex;

    /** True while the player is holding the zoom input (right mouse button). */
    UPROPERTY(Transient)
    bool bWantsZoom = false;

    /** Current zoom alpha [0..1] interpolated over time. */
    UPROPERTY(Transient)
    float CurrentZoomAlpha = 0.f;

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

    /** Tick used to update smooth zoom behaviour on the active camera. */
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

    /** Suspends camera updates (parallax/zoom). Useful when opening the map. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera")
    void SetSuspended(bool bInSuspended);

    /** Returns whether the camera manager is suspended. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera")
    bool IsSuspended() const { return bSuspended; }

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
    /** Sets whether the player currently wants to zoom (right mouse button held). */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera|Zoom")
    void SetWantsZoom(bool bInWantsZoom);

protected:
    /** Applies smooth zoom on the active camera each tick. */
    void UpdateZoom(float DeltaTime);

#pragma endregion ZOOM

#pragma region INTERNAL

protected:
    /** Sorts the list of cameras by their CameraIndex property. */
    void SortCameraSpots();

    /** Returns the owning player controller, if any. */
    APlayerController* GetOwningPlayerController() const;

#pragma endregion INTERNAL

private:
    /** If true, camera updates are suspended (map opened). */
    bool bSuspended = false;
};
