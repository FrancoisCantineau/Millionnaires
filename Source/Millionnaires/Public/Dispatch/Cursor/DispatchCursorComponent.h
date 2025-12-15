/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchCursorComponent" - Header
 * Notes: Handles cursor traces, hover logic, radial charge and global slow motion.
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DispatchCursorComponent.generated.h"

class ADispatchPlayerController;

#pragma region DELEGATES

/** Legacy progress delegate: kept for compatibility. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDispatchHoverProgress, float, Progress, bool, bIsHoveringCharacter);

/** UI-friendly hover delegate: includes screen position and visibility. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDispatchHoverUIProgress, float, Progress, FVector2D, ScreenPos, bool, bVisible);

/** Click delegate (hovered actor). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDispatchActorClicked, AActor*, ClickedActor);

#pragma endregion DELEGATES

/**
 * Cursor component responsibilities:
 * - trace under cursor
 * - compute hover state
 * - compute a "hold" radial progress (0..1) when hovering APawn or actors tagged HoverableTag
 * - optional slow motion while hovering characters
 *
 * UI should bind to OnHoverUIProgress (new).
 */
UCLASS(ClassGroup=(Dispatch), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UDispatchCursorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
#pragma region LIFECYCLE

    /** Default constructor. */
    UDispatchCursorComponent();

    /** BeginPlay. */
    virtual void BeginPlay() override;

    /** EndPlay. */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Tick. */
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion LIFECYCLE

#pragma region API

    /** Called by the player controller when the user clicks (mouse left button). */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Cursor")
    void HandleClick();

    /** Returns the actor currently hovered by the cursor, if any. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Cursor")
    AActor* GetCurrentHoveredActor() const { return HoveredActor.Get(); }

    /** Returns last known cursor position on screen (in pixels). */
    UFUNCTION(BlueprintPure, Category="Dispatch|Cursor")
    FVector2D GetLastCursorScreenPos() const { return LastMousePos; }


    /** Enables/disables world cursor interactions (trace + slowmo). Useful when opening the map. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Cursor")
    void SetWorldCursorEnabled(bool bEnabled);

    /** Returns whether world cursor interactions are enabled. */
    UFUNCTION(BlueprintPure, Category="Dispatch|Cursor")
    bool IsWorldCursorEnabled() const { return bWorldCursorEnabled; }

#pragma endregion API

#pragma region EVENTS

    /** Legacy delegate broadcast whenever the radial hover progress is updated this frame. */
    UPROPERTY(BlueprintAssignable, Category = "Dispatch|Cursor|Events")
    FOnDispatchHoverProgress OnHoverProgress;

    /** New UI delegate with screen position and visibility. */
    UPROPERTY(BlueprintAssignable, Category="Dispatch|Cursor|Events")
    FOnDispatchHoverUIProgress OnHoverUIProgress;

    /** Fired when user clicks (with hovered actor). */
    UPROPERTY(BlueprintAssignable, Category="Dispatch|Cursor|Events")
    FOnDispatchActorClicked OnActorClicked;

#pragma endregion EVENTS

#pragma region CONFIG

protected:
    /** Maximum distance for the cursor line trace. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Cursor|Trace")
    float TraceDistance = 200000.f;

    /** Collision channel used for the cursor trace. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Cursor|Trace")
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

    /** If the actor has this tag, we treat it as hoverable for radial fill. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Cursor|Hover")
    FName HoverableTag = FName("DispatchHoverable");

    /** Time (seconds) for the radial hover to fill from 0->1. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Cursor|Hover", meta=(ClampMin="0.05"))
    float HoverFillTime = 0.6f;

    /** Slow-mo target time dilation when hovering a character at full progress. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Cursor|SlowMo", meta=(ClampMin="0.01", ClampMax="1.0"))
    float SlowMoTargetDilation = 0.25f;

    /** Interp speed for time dilation transitions. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Cursor|SlowMo", meta=(ClampMin="0.1"))
    float SlowMoInterpSpeed = 8.f;

#pragma endregion CONFIG

#pragma region INTERNAL

protected:
    /** Performs a line trace under the cursor and returns the first hit actor (or nullptr). */
    AActor* PerformHoverTrace();

    /** Updates the radial charge and slow motion based on the actor hovered this frame. */
    void UpdateHoverAndSlowMo(float DeltaTime, AActor* NewHoveredActor);

    /** Sets the global time dilation safely. */
    void ApplyGlobalTimeDilation(float NewDilation);

#pragma endregion INTERNAL

private:
#pragma region STATE

    /** Cached controller owning this component. */
    TWeakObjectPtr<ADispatchPlayerController> CachedController;

    /** Actor currently hovered by cursor. */
    TWeakObjectPtr<AActor> HoveredActor;

    /** Current hover fill (0..1). */
    float CurrentHoverProgress = 0.f;

    /** Current global time dilation (smoothed). */
    float CurrentTimeDilation = 1.f;

    /** Enables/disables tracing/slowmo (disabled while map is open). */
    bool bWorldCursorEnabled = true;

    /** Last known mouse position. */
    FVector2D LastMousePos = FVector2D::ZeroVector;

#pragma endregion STATE
};
