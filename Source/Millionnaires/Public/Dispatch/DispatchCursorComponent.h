/**
 * Millionnaires Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchCursorComponent"
 * Notes: Handles cursor traces, hover logic, radial charge and global slow motion.
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DispatchCursorComponent.generated.h"

class ADispatchPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDispatchHoverProgress, float, Progress, bool, bIsHoveringCharacter);

/**
 * Component attached to the Dispatch player controller.
 * It performs a line trace under the mouse cursor, keeps track of the hovered actor,
 * drives a radial "charge" value over time and applies global time dilation when hovering characters.
 */
UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class MILLIONNAIRES_API UDispatchCursorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /** Default constructor. */
    UDispatchCursorComponent();

#pragma region CONFIG

protected:
    /** Maximum distance for the cursor line trace. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Cursor")
    float TraceDistance;

    /** Collision channel used for the cursor trace. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Cursor")
    TEnumAsByte<ECollisionChannel> TraceChannel;

    /** Tag that marks actors as "hoverable" for the radial charge (in addition to being a Pawn). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Cursor")
    FName HoverableTag;
    /** Time in seconds required to fully fill the radial hover circle. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|SlowMo")
    float HoverFillTime;

    /** Target global time dilation when the radial is fully filled over a character. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|SlowMo")
    float SlowMoTargetDilation;

    /** How fast the global time dilation interpolates towards the target. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|SlowMo")
    float SlowMoInterpSpeed;

#pragma endregion CONFIG

#pragma region RUNTIME_STATE

protected:
    /** Owning Dispatch player controller (cached for convenience). */
    UPROPERTY(Transient)
    TWeakObjectPtr<ADispatchPlayerController> CachedController;

    /** Currently hovered actor under the mouse cursor. */
    UPROPERTY(Transient)
    TWeakObjectPtr<AActor> HoveredActor;

    /** Current radial fill progress in [0,1]. */
    UPROPERTY(Transient)
    float CurrentHoverProgress;

    /** Current global time dilation used while hovering. */
    UPROPERTY(Transient)
    float CurrentTimeDilation;

#pragma endregion RUNTIME_STATE

#pragma region LIFECYCLE

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion LIFECYCLE

#pragma region API

public:
    /** Called by the player controller when the user clicks (mouse left button). */
    UFUNCTION(BlueprintCallable, Category = "Dispatch|Cursor")
    void HandleClick();

    /** Returns the actor currently hovered by the cursor, if any. */
    UFUNCTION(BlueprintPure, Category = "Dispatch|Cursor")
    AActor* GetCurrentHoveredActor() const { return HoveredActor.Get(); }

    /** Delegate broadcast whenever the radial hover progress is updated this frame. */
    UPROPERTY(BlueprintAssignable, Category = "Dispatch|Cursor")
    FOnDispatchHoverProgress OnHoverProgress;

#pragma endregion API

#pragma region INTERNAL

protected:
    /** Performs a line trace under the cursor and returns the first hit actor (or nullptr). */
    AActor* PerformHoverTrace();

    /** Updates the radial charge and slow motion based on the actor hovered this frame. */
    void UpdateHoverAndSlowMo(float DeltaTime, AActor* NewHoveredActor);

    /** Sets the global time dilation safely. */
    void ApplyGlobalTimeDilation(float NewDilation);

#pragma endregion INTERNAL
};
