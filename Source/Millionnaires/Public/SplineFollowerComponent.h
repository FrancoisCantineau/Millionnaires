// SplineFollowerComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SplineFollowerComponent.generated.h"

class USplineComponent;
class UCurveFloat;
class AMetroStation;
class AMetroWagon;

UENUM(BlueprintType)
enum class ESplineFollowerState : uint8
{
	Accelerating,
	Cruising,
	Braking,
	DoorsOpening,
	Dwelling,
	DoorsClosing,
	EmergencyBraking,
	EmergencyStopped
};

/**
 * Moves its owning Actor along a spline, handling acceleration, braking into
 * stations, and dwell time. Used for metro/train cars (and, generically, any
 * actor that should travel along a fixed path with stops).
 *
 * Does NOT handle passenger attachment — a Character standing on a Movable
 * component owned by this Actor will automatically ride along via Unreal's
 * native MovementBase system, as long as this component moves the owner via
 * SetActorTransform (not AddActorWorldOffset) to avoid drift.
 *
 * Door animation/opening is NOT handled here — OnArrivedAtStation and
 * OnDepartingStation are BlueprintImplementableEvents meant to drive doors,
 * sound, etc. from Blueprint, since the door setup (meshes/anims) is project-specific.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MILLIONNAIRES_API USplineFollowerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USplineFollowerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Starts the follower moving (no-op if already moving). */
	UFUNCTION(BlueprintCallable, Category = "SplineFollower")
	void StartMoving();

	/** Stops the follower in place. */
	UFUNCTION(BlueprintCallable, Category = "SplineFollower")
	void StopMoving();

	UFUNCTION(BlueprintPure, Category = "SplineFollower")
	ESplineFollowerState GetState() const { return State; }

	/** Fired whenever the state machine transitions — hook brake screech/sparks on entering
	 *  Braking/EmergencyBraking, an acceleration/traction sound on Accelerating, an ambient
	 *  cruising loop on Cruising, etc. Fires once per transition, not every Tick. */
	UFUNCTION(BlueprintImplementableEvent, Category = "SplineFollower")
	void OnStateChanged(ESplineFollowerState OldState, ESplineFollowerState NewState);

	/** Current distance traveled along the spline, in cm. Used by AMetroWagon to compute its own trailing position. */
	UFUNCTION(BlueprintPure, Category = "SplineFollower")
	float GetDistanceAlongSpline() const { return DistanceAlongSpline; }

	/** The resolved spline component this follower is moving along (may be null before BeginPlay runs). */
	UFUNCTION(BlueprintPure, Category = "SplineFollower")
	USplineComponent* GetSplineComponent() const { return TargetSpline; }

	/** Called by UMetroLineComponent when a matching incident triggers/resolves. If the train is
	 *  currently at a station, it stays there with doors open. If it's mid-transit, it emergency-
	 *  brakes immediately wherever it is on the spline and opens its doors there. */
	UFUNCTION(BlueprintCallable, Category = "SplineFollower")
	void SetHeldByIncident(bool bHeld);

	/** Fired when the train comes to a full emergency stop mid-transit (not at a station) —
	 *  implement in Blueprint for tunnel emergency lighting, forcing doors open, etc. */
	UFUNCTION(BlueprintImplementableEvent, Category = "SplineFollower")
	void OnEmergencyStop();

	/** Fired when an emergency stop ends and the train is about to resume moving. */
	UFUNCTION(BlueprintImplementableEvent, Category = "SplineFollower")
	void OnEmergencyResume();

	/** Estimated seconds until this train's doors open at Station (searches forward through this
	 *  train's remaining stops on the line). Returns -1 if Station isn't on this train's route.
	 *  Approximate: assumes CruiseSpeed for travel time on each leg rather than modeling the
	 *  accel/brake ramp precisely — fine for a UI countdown, not frame-exact. */
	UFUNCTION(BlueprintCallable, Category = "SplineFollower")
	float GetEstimatedTimeToStation(AMetroStation* Station) const;

	/** Debug helper: the station this train is currently heading toward. Use to verify the
	 *  correct starting target was picked when a train starts partway along the line. */
	UFUNCTION(BlueprintPure, Category = "SplineFollower|Debug")
	AMetroStation* GetNextStation() const { return Stations.IsValidIndex(NextStationIndex) ? Stations[NextStationIndex].Get() : nullptr; }

	/** Fired the instant the train comes to a full stop at a station — implement in Blueprint to start the door-opening animation. */
	UFUNCTION(BlueprintImplementableEvent, Category = "SplineFollower")
	void OnArrivedAtStation(AMetroStation* Station);

	/** Fired the instant doors should start closing (dwell time has elapsed) — implement in Blueprint to start the door-closing animation.
	 *  The train does not actually start accelerating until DoorCloseDuration has elapsed after this fires. */
	UFUNCTION(BlueprintImplementableEvent, Category = "SplineFollower")
	void OnDepartingStation(AMetroStation* Station);

protected:
	/** The Actor that owns the spline this component follows (e.g. a level Actor representing a metro line).
	 *  Multiple SplineFollowerComponents (multiple trains) can point at the same SplineOwner. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower")
	TObjectPtr<AActor> SplineOwner;

	/** Where this train starts on the spline, in cm. Set different values per train instance on the
	 *  same line so multiple trains don't all start stacked at distance 0. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower")
	float InitialDistanceAlongSpline = 0.f;

	/** If true, ignores stations entirely and loops at constant CruiseSpeed (simple shuttle behavior). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower")
	bool bLoopsContinuously = false;

	/** Cruising speed once fully accelerated, in cm/s. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower")
	float CruiseSpeed = 800.f;

	/** Acceleration rate, in cm/s^2. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower", meta = (EditCondition = "!bLoopsContinuously"))
	float AccelRate = 400.f;

	/** Deceleration rate, in cm/s^2, used when no SpeedCurve is assigned. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower", meta = (EditCondition = "!bLoopsContinuously"))
	float BrakeRate = 500.f;

	/** Distance from a station, in cm, at which braking begins. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower", meta = (EditCondition = "!bLoopsContinuously"))
	float BrakeDistance = 1500.f;

	/** Optional curve mapping [0..1] = normalized distance-to-station -> [0..1] speed factor (applied to CruiseSpeed).
	 *  If unset, falls back to the linear BrakeRate behavior. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower", meta = (EditCondition = "!bLoopsContinuously"))
	TObjectPtr<UCurveFloat> SpeedCurve;

	/** Stations along this line, sorted by distance along the spline. Auto-discovered at BeginPlay if empty. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower", meta = (EditCondition = "!bLoopsContinuously"))
	TArray<TObjectPtr<AMetroStation>> Stations;

	/** How long doors take to fully open before dwelling begins, in seconds. Train property (same for all stations on this line). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower", meta = (EditCondition = "!bLoopsContinuously"))
	float DoorOpenDuration = 2.f;

	/** How long doors take to fully close before the train starts accelerating away, in seconds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower", meta = (EditCondition = "!bLoopsContinuously"))
	float DoorCloseDuration = 2.f;

	/** Trailing wagons for this train, in order from the one right behind the head to the last one.
	 *  Each wagon follows the element directly in front of it (chain), updated in this order every Tick. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower")
	TArray<TObjectPtr<AMetroWagon>> Wagons;

private:
	/** Resolved from SplineOwner in BeginPlay — cached so we don't call FindComponentByClass every Tick. */
	UPROPERTY(Transient)
	TObjectPtr<USplineComponent> TargetSpline;

	/** Current distance traveled along the spline, in cm. */
	float DistanceAlongSpline = 0.f;

	/** Current speed, in cm/s. */
	float CurrentSpeed = 0.f;

	ESplineFollowerState State = ESplineFollowerState::Accelerating;

	bool bIsMoving = false;

	/** Index into Stations of the next station ahead (wraps around). */
	int32 NextStationIndex = 0;

	/** Countdown for the current phase while stopped (DoorsOpening/Dwelling/DoorsClosing). */
	float PhaseTimeRemaining = 0.f;

	/** Set by UMetroLineComponent during an active incident. Pauses progression out of Dwelling. */
	bool bHeldByIncident = false;

	/** The line's UMetroLineComponent (if any), found on SplineOwner at BeginPlay — cached for unregistering at EndPlay. */
	UPROPERTY(Transient)
	TObjectPtr<class UMetroLineComponent> CachedLineComponent;

	/** Auto-discovers all AMetroStation actors attached to / near SplineOwner and sorts them by spline distance. */
	void DiscoverAndSortStations();

	/** Advances DistanceAlongSpline per DeltaTime and applies the resulting transform to the owner. */
	void AdvanceAlongSpline(float DeltaTime);

	/** Runs the Accelerating/Cruising/Braking/Stopped state machine, updating CurrentSpeed and State. */
	void UpdateStateMachine(float DeltaTime, float SplineLength);

	/** Distance from DistanceAlongSpline to the next station ahead, accounting for spline wraparound. */
	float GetDistanceToNextStation(float SplineLength) const;
};