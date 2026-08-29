// MetroWagon.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MetroWagon.generated.h"

class USplineComponent;

/**
 * A trailing wagon for a metro/train. Has no movement logic of its own and does
 * NOT Tick independently — the head-of-train's SplineFollowerComponent drives
 * UpdateFollow() on each wagon in chain order (head -> wagon 1 -> wagon 2 -> ...)
 * every frame, after updating its own position. This guarantees each wagon reads
 * an already-updated "distance ahead of it" this frame, avoiding the one-frame-lag
 * risk of independent Actor Ticks with unspecified relative order.
 *
 * Each wagon follows the element directly in front of it (chain topology), not
 * the head directly — so braking/accelerating "ripples" backward through the
 * train like a real coupling, rather than every wagon reacting identically to
 * the head at once.
 */
UCLASS()
class MILLIONNAIRES_API AMetroWagon : public AActor
{
	GENERATED_BODY()

public:
	AMetroWagon();

	/**
	 * Called by the head (or the preceding wagon's owner logic) once per frame, in chain order.
	 * @param DeltaTime           Frame delta time.
	 * @param PrecedingDistance   Current spline distance of the element directly in front of this wagon.
	 * @param Spline              The spline both this wagon and the preceding element are moving along.
	 * @return This wagon's own resulting spline distance, for the next wagon in the chain to use as its PrecedingDistance.
	 */
	float UpdateFollow(float DeltaTime, float PrecedingDistance, USplineComponent* Spline);

	/** Fired on this wagon when the train's doors should open — implement in this wagon's Blueprint to play its own door animation. */
	UFUNCTION(BlueprintImplementableEvent, Category = "MetroWagon")
	void OnDoorsShouldOpen();

	/** Fired on this wagon when the train's doors should close. */
	UFUNCTION(BlueprintImplementableEvent, Category = "MetroWagon")
	void OnDoorsShouldClose();

protected:
	/** Distance behind the preceding element (head or previous wagon) this wagon wants to sit at, in cm. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetroWagon")
	float DesiredDistanceBehind = 800.f;

	/** If true, follows with spring-like lag (FollowStiffness). If false, snaps rigidly to the exact distance every frame. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetroWagon")
	bool bUseElasticity = true;

	/** Higher = snappier/less lag, lower = more elastic "coupling" feel. Only used when bUseElasticity is true. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetroWagon", meta = (EditCondition = "bUseElasticity"))
	float FollowStiffness = 6.f;

private:
	/** This wagon's own current distance along the spline. */
	float CurrentDistance = 0.f;

	bool bInitialized = false;
};