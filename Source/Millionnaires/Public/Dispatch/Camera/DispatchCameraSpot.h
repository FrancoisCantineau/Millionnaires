/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchCameraSpot" - Header
 * Notes: Camera actor placed in the control room and used as Dispatch viewpoints.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "DispatchCameraSpot.generated.h"

class USceneComponent;
class UCameraComponent;

/**
 * Simple camera spot the DispatchPlayerController can blend to when cycling cameras.
 */
UCLASS()
class MILLIONNAIRES_API ADispatchCameraSpot : public AActor
{
    GENERATED_BODY()

public:
    /** Default constructor. */
    ADispatchCameraSpot();

#pragma region COMPONENTS

protected:
    /** Root scene component for this camera spot. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera", meta = (AllowPrivateAccess = "true"))
    USceneComponent* Root;

    /** The camera used when this spot becomes the active Dispatch view target. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* Camera;

#pragma endregion COMPONENTS

#pragma region CONFIG

protected:
    /** If true, this camera is considered the Map camera (used to auto-reset map state when leaving it). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Map")
    bool bIsMapCamera = false;
    
    /** Optional index used when ordering cameras manually (e.g. for cycling left/right). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera")
    int32 CameraIndex = 0;

    /** Blend time used when switching to this camera. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera")
    float BlendTime = 0.5f;

    /** If true, this camera allows zoom on right mouse button. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Zoom")
    bool bAllowZoom = true;

    /** Target FOV when fully zoomed in. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Zoom")
    float ZoomedFOV = 40.f;

    /** Interp speed used when zooming in. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Zoom")
    float ZoomInterpSpeedIn = 6.f;

    /** Interp speed used when zooming out. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Zoom")
    float ZoomInterpSpeedOut = 6.f;

    /** If true, apply extra postprocess when zoomed. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Zoom")
    bool bUseZoomPostProcess = false;

    /** Overall weight multiplier for the zoom postprocess effect. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Zoom", meta = (EditCondition = "bUseZoomPostProcess"))
    float ZoomPostProcessBlendWeight = 1.f;

    /** Postprocess settings applied when zoom blend is > 0. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Zoom", meta = (EditCondition = "bUseZoomPostProcess"))
    FPostProcessSettings ZoomPostProcessSettings;

#pragma endregion CONFIG

#pragma region LIFECYCLE

protected:
    /** Called when the game starts or when spawned. */
    virtual void BeginPlay() override;

public:
    /** Called when this actor is being destroyed. */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma endregion LIFECYCLE

#pragma region STATE

protected:
    /** Cached FOV used when not zoomed, captured at BeginPlay. */
    UPROPERTY(Transient)
    float InitialFOV = 90.f;

    /** Cached camera relative rotation captured at BeginPlay (neutral parallax pose). */
    UPROPERTY(Transient)
    FRotator InitialCameraRelativeRotation = FRotator::ZeroRotator;

    /** Cached camera relative location captured at BeginPlay (neutral parallax pose). */
    UPROPERTY(Transient)
    FVector InitialCameraRelativeLocation = FVector::ZeroVector;

#pragma endregion STATE

#pragma region MOUSE_PARALLAX_CONFIG

protected:
    /** If true, the active camera will slightly pan/tilt toward the mouse position. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Mouse Parallax")
    bool bUseMouseParallax = true;

    /** Max yaw offset in degrees when the mouse is at screen edges. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Mouse Parallax", meta = (ClampMin = "0.0"))
    float MouseParallaxMaxYaw = 4.f;

    /** Max pitch offset in degrees when the mouse is at screen edges. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Mouse Parallax", meta = (ClampMin = "0.0"))
    float MouseParallaxMaxPitch = 2.f;

    /** Optional local location offset (Y/Z usually) when mouse is at screen edges. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Mouse Parallax")
    FVector MouseParallaxMaxLocationOffset = FVector(0.f, 10.f, 5.f);

    /** Interp speed used to smooth the parallax movement. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Mouse Parallax", meta = (ClampMin = "0.0"))
    float MouseParallaxInterpSpeed = 8.f;

    /** Deadzone around the screen center to avoid jitter (0..0.5). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Mouse Parallax", meta = (ClampMin = "0.0", ClampMax = "0.5"))
    float MouseParallaxDeadZone = 0.05f;

    /** If true, invert vertical parallax direction. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Camera|Mouse Parallax")
    bool bInvertMouseParallaxY = false;

#pragma endregion MOUSE_PARALLAX_CONFIG

#pragma region API

public:
    /** Returns true if this spot is the Map camera. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Map")
    bool IsMapCamera() const { return bIsMapCamera; }
    
    /** Returns the internal camera component. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera")
    UCameraComponent* GetCameraComponent() const { return Camera; }

    /** Returns the camera index used for ordering. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera")
    int32 GetCameraIndex() const { return CameraIndex; }

    /** Returns the blend time used when activating this camera. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera")
    float GetBlendTime() const { return BlendTime; }

    /** Registers this camera spot in the active Dispatch camera manager if possible. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera")
    void RegisterToDispatchCameraManager();

    /** Unregisters this camera spot from the active Dispatch camera manager if possible. */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Camera")
    void UnregisterFromDispatchCameraManager();

        /** Returns true if zoom is allowed for this camera. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Zoom")
    bool IsZoomAllowed() const { return bAllowZoom; }

    /** Returns the initial (un-zoomed) FOV captured at BeginPlay. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Zoom")
    float GetInitialFOV() const { return InitialFOV; }

    /** Returns target FOV when fully zoomed. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Zoom")
    float GetZoomedFOV() const { return ZoomedFOV; }

    /** Returns zoom-in interp speed. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Zoom")
    float GetZoomInterpSpeedIn() const { return ZoomInterpSpeedIn; }

    /** Returns zoom-out interp speed. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Zoom")
    float GetZoomInterpSpeedOut() const { return ZoomInterpSpeedOut; }

    /** Returns true if this camera uses zoom postprocess. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Zoom")
    bool UsesZoomPostProcess() const { return bUseZoomPostProcess; }

    /** Returns zoom postprocess settings. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Zoom")
    const FPostProcessSettings& GetZoomPostProcessSettings() const { return ZoomPostProcessSettings; }

    /** Returns target blend weight for the zoom postprocess. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Zoom")
    float GetZoomPostProcessBlendWeight() const { return ZoomPostProcessBlendWeight; }

    #pragma region MOUSE_PARALLAX_API

    public:
        /** Returns true if this camera uses mouse parallax. */
        UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Mouse Parallax")
        bool UsesMouseParallax() const { return bUseMouseParallax; }

        UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Mouse Parallax")
        float GetMouseParallaxMaxYaw() const { return MouseParallaxMaxYaw; }

        UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Mouse Parallax")
        float GetMouseParallaxMaxPitch() const { return MouseParallaxMaxPitch; }

        UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Mouse Parallax")
        FVector GetMouseParallaxMaxLocationOffset() const { return MouseParallaxMaxLocationOffset; }

        UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Mouse Parallax")
        float GetMouseParallaxInterpSpeed() const { return MouseParallaxInterpSpeed; }

        UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Mouse Parallax")
        float GetMouseParallaxDeadZone() const { return MouseParallaxDeadZone; }

        UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Mouse Parallax")
        bool IsMouseParallaxYInverted() const { return bInvertMouseParallaxY; }

        UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Mouse Parallax")
        FRotator GetInitialCameraRelativeRotation() const { return InitialCameraRelativeRotation; }

        UFUNCTION(BlueprintPure, Category = "Dispatch|Camera|Mouse Parallax")
        FVector GetInitialCameraRelativeLocation() const { return InitialCameraRelativeLocation; }

    #pragma endregion MOUSE_PARALLAX_API

#pragma endregion API
};
