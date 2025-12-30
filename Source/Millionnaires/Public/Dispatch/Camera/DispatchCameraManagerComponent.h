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
#include "Engine/EngineTypes.h"
#include "Dispatch/DispatchTypes.h"
#include "DispatchCameraManagerComponent.generated.h"

class ADispatchCameraSpot;
class APlayerController;
class UCameraComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

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

    /** Default post process material used for camera transitions (optional). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Transition")
    TObjectPtr<UMaterialInterface> DefaultTransitionPostProcessMaterial = nullptr;

    /** Default duration (seconds) for the transition post process. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Transition", meta = (ClampMin = "0.0"))
    float DefaultTransitionPostProcessDuration = 0.25f;

    /** Default blend weight for the transition post process. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Transition", meta = (ClampMin = "0.0"))
    float DefaultTransitionPostProcessWeight = 1.f;

    /** If true, the initial camera will try to avoid the Map camera (bIsMapCamera). */
    UPROPERTY(EditAnywhere, Category = "Dispatch|Camera")
    bool bPreferNonMapAtStartup = true;

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

    /** Virtual cursor normalized X [0..1], used when the real cursor is captured/hidden. */
    UPROPERTY(Transient)
    float VirtualCursorX01 = 0.5f;

    /** Virtual cursor normalized Y [0..1], used when the real cursor is captured/hidden. */
    UPROPERTY(Transient)
    float VirtualCursorY01 = 0.5f;

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
    /** Activates the Map camera (the one flagged with bIsMapCamera). */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera|Map")
    bool ActivateMapCamera(bool bUseTransition = true);

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

    /** Returns the index of the currently active camera in the internal list, or INDEX_NONE. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera")
    int32 GetActiveCameraIndex() const { return ActiveCameraIndex; }

    /** Returns the currently active camera spot. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera")
    ADispatchCameraSpot* GetActiveCamera() const;

#pragma endregion CAMERA_CONTROL

#pragma region MOUSE_PARALLAX

protected:
    /** Applies subtle camera parallax toward the mouse for the active camera. */
    void UpdateMouseParallax(float DeltaTime);

    /** Resets the active camera back to its neutral parallax pose. */
    void ResetMouseParallax(ADispatchCameraSpot* CameraSpot);

#pragma endregion MOUSE_PARALLAX

#pragma region ZOOM

public:
    /** Sets whether the player currently wants to zoom (right mouse button held). */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera|Zoom")
    void SetWantsZoom(bool bInWantsZoom);

protected:
    /** Applies smooth zoom on the active camera each tick. */
    void UpdateZoom(float DeltaTime);

#pragma endregion ZOOM

#pragma region CAMERA_TRANSITION

public:
    /** Switches camera AND plays a post-process overlay for a fixed duration (applied on both old & new cameras). */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera|Transition")
    void ActivateCameraByIndexWithPostProcessTransition(int32 Index, UMaterialInterface* PostProcessMaterial, float Duration, float Weight = 1.f);

    /** Plays a post-process overlay for a fixed duration without switching cameras. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera|Transition")
    void PlayPostProcessTransition(UMaterialInterface* PostProcessMaterial, float Duration, float Weight = 1.f);

    /** Stops the current post-process transition overlay (if any). */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera|Transition")
    void StopPostProcessTransition();

protected:
    /** Updates and stops the active post-process transition overlay when time is over. */
    void UpdatePostProcessTransition(float DeltaTime);

private:
    /** Backup of camera post process state we modify for the transition. */
    struct FDispatchCameraPPBackup
    {
        TWeakObjectPtr<UCameraComponent> Camera;

        /** Saved camera blend weight (we may force it to 1.0 during transition). */
        float SavedPostProcessBlendWeight = 0.f;

        /** Saved weighted blendables (we inject our PP material here). */
        FWeightedBlendables SavedBlendables;
    };

    /** Dynamic instance used for the transition post process. */
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> ActiveTransitionPP = nullptr;

    /** Remaining time (seconds) for the current transition overlay. */
    float ActiveTransitionPPRemaining = 0.f;

    /** Blend weight for the current transition overlay. */
    float ActiveTransitionPPWeight = 1.f;

    /** Prevent "start & stop in same frame" (ensures at least one rendered frame). */
    bool bTransitionSkipFirstTick = false;
    
    /** Backups of extra PP blends for cameras affected by the transition. */
    TArray<FDispatchCameraPPBackup> ActiveTransitionPPBackups;

#pragma endregion CAMERA_TRANSITION

#pragma region INTERNAL

protected:
    /** Returns the owning player controller, if any. */
    APlayerController* GetOwningPlayerController() const;
    
    /** Sorts the list of cameras by their CameraIndex property. */
    void SortCameraSpots();

    /** Returns the index of the first camera flagged as Map camera, or INDEX_NONE if not found. */
    int32 FindMapCameraIndex() const;

    /** Returns the next camera index that can be cycled to (skips Map camera), or INDEX_NONE if none found. */
    int32 FindNextCyclableCameraIndex(int32 FromIndex, int32 Direction) const;

    /** Returns which camera index should be used at startup (can skip the Map camera). */
    int32 FindStartupCameraIndex() const;

#pragma endregion INTERNAL

private:
    /** If true, camera updates are suspended (map opened). */
    bool bSuspended = false;
};
