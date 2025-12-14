/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchTypes" - Header
 * Notes: Shared enums, structs and delegates for the Dispatch (Control Room) systems.
 */

#pragma once

#include "CoreMinimal.h"
#include "DispatchTypes.generated.h"

class AActor;
class ADispatchCameraSpot;

/** Type of camera switch requested for the current fade sequence. */
UENUM()
enum class EDispatchPendingCameraSwitch : uint8
{
    None,
    Left,
    Right
};

/**
 * High level state of the Dispatch time manipulation logic.
 */
UENUM(BlueprintType)
enum class EDispatchTimeState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Slowing     UMETA(DisplayName = "Slowing"),
    Stopped     UMETA(DisplayName = "Stopped"),
    Recovering  UMETA(DisplayName = "Recovering")
};

/**
 * Tunable settings for the time dilation behaviour while hovering characters in Dispatch.
 */
USTRUCT(BlueprintType)
struct MILLIONNAIRES_API FDispatchTimeSettings
{
    GENERATED_BODY()

    /** Duration (in seconds) to go from normal speed to full stop while hovering a character. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispatch|Time")
    float TimeToFullStop = 1.0f;

    /** Duration (in seconds) to smoothly return from the current time dilation back to normal speed. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispatch|Time")
    float RecoveryDuration = 0.5f;

    /** Time dilation value reached when the hover has fully completed (0 = full stop, 0.01-0.05 recommended). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispatch|Time")
    float MinTimeDilation = 0.01f;

    /** If true, the settings apply using global time dilation, otherwise they will not change time at all. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispatch|Time")
    bool bAffectsGlobalTime = true;
};

/** Fired when the cursor hover target changes in Dispatch. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchHoverActorChanged, AActor*, NewActor);

/** Fired when the current inspectable (character) target changes in Dispatch. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchInspectTargetChanged, AActor*, NewTarget);

/** Fired whenever the Dispatch time dilation value is updated. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDispatchTimeDilationChanged, float, NewTimeDilation, float, PreviousTimeDilation);

/** Fired when the Dispatch time state changes (Idle/Slowing/Stopped/Recovering). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchTimeStateChanged, EDispatchTimeState, NewState);

/** Fired when the active Dispatch camera changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchCameraChanged, ADispatchCameraSpot*, NewCameraSpot);
