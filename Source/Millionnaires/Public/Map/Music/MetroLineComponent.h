// MetroLineComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Map/Events/IncidentManager.h"
#include "MetroLineComponent.generated.h"

class USplineFollowerComponent;
class AMetroStation;

USTRUCT(BlueprintType)
struct FMetroArrivalInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USplineFollowerComponent> Train;

	/** Estimated seconds until this train's doors open at the queried station. Approximate —
	 *  assumes cruise speed for travel time and does not model accel/brake ramp precisely,
	 *  which is fine for a UI countdown. */
	UPROPERTY(BlueprintReadOnly)
	float EstimatedSeconds = 0.f;
};

/**
 * Placed on the same Actor as a metro line's spline (the "SplineOwner" trains reference).
 * Keeps a registry of all trains currently running the line, answers arrival-ETA queries for
 * station display panels, and reacts to UIncidentManager the same way ULightBaseComponent does:
 * on a matching incident, held trains finish their current approach normally and then wait at
 * their next station with doors open (never frozen mid-track) until the incident resolves.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MILLIONNAIRES_API UMetroLineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMetroLineComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/** Called by each SplineFollowerComponent on this line during its own BeginPlay/EndPlay. */
	void RegisterTrain(USplineFollowerComponent* Train);
	void UnregisterTrain(USplineFollowerComponent* Train);

	/** Up to MaxCount upcoming arrivals at Station across all registered trains, soonest first. */
	UFUNCTION(BlueprintCallable, Category = "MetroLine")
	TArray<FMetroArrivalInfo> GetNextArrivals(AMetroStation* Station, int32 MaxCount = 2) const;

	/** Identifies this line for incident zone-matching (mirrors ULightBaseComponent's ZoneType). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetroLine")
	FGameplayTag ZoneType;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<USplineFollowerComponent>> RegisteredTrains;

	void SubscribeToIncidentManager();
	void UnsubscribeFromIncidentManager();

	UFUNCTION()
	void OnIncidentTriggeredDelegate(FShipIncident Incident);

	UFUNCTION()
	void OnIncidentResolvedDelegate(FShipIncident Incident);
};
