// LookAtComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LookAtComponent.generated.h"

/**
 * Reusable "where should this character look" component — add it to ANY Pawn/Character,
 * regardless of class hierarchy or skeleton, and wire your AnimGraph's Look At node to
 * GetLookAtTarget()/GetLookAtAlpha(). BT tasks (FirstReaction, LookAround, etc.) just call
 * SetLookAtTarget()/ClearLookAtTarget() — they never touch bones, animation, or rotation directly.
 *
 * GetLookAtAlpha() smoothly ramps 0->1 when a target is set and 1->0 when cleared, so the
 * AnimGraph can fade the look-at in/out gracefully instead of snapping.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MILLIONNAIRES_API ULookAtComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULookAtComponent();

	/** Sets the world-space point to look at and starts fading the blend weight toward 1. The
	 *  reported target (GetLookAtTarget) doesn't snap here instantly — it eases toward this point
	 *  at TargetLagSpeed, giving a natural "catching up" feel instead of an instant snap. */
	UFUNCTION(BlueprintCallable, Category = "LookAt")
	void SetLookAtTarget(FVector WorldLocation);

	/** Starts fading the blend weight toward 0. The last target location is kept (not zeroed)
	 *  until the next SetLookAtTarget call, so the head doesn't snap to the origin mid-fade-out. */
	UFUNCTION(BlueprintCallable, Category = "LookAt")
	void ClearLookAtTarget();

	/** The smoothed, lagged target — feed this into your AnimGraph, not the raw requested point. */
	UFUNCTION(BlueprintPure, Category = "LookAt")
	FVector GetLookAtTarget() const { return SmoothedTarget; }

	/** Smoothed 0-1 blend weight — feed this into your AnimGraph Look At node's Alpha pin. */
	UFUNCTION(BlueprintPure, Category = "LookAt")
	float GetLookAtAlpha() const { return CurrentAlpha; }

	/** How fast the blend weight ramps up/down, in units per second (1.0 = full fade in ~1s). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt")
	float AlphaBlendSpeed = 2.f;

	/** How fast the reported target eases toward the actual requested point — lower values feel
	 *  more like a heavy, deliberate creature; higher values feel snappier/more alert. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt")
	float TargetLagSpeed = 6.f;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FVector DesiredTarget = FVector::ZeroVector;
	FVector SmoothedTarget = FVector::ZeroVector;
	float CurrentAlpha = 0.f;
	bool bWantsLookAt = false;
};