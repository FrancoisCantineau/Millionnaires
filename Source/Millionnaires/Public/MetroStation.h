// MetroStation.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MetroStation.generated.h"

class UMetroLineComponent;

/**
 * A placeable stop point for a metro/train line.
 * Place it wherever you want visually along the track — it projects itself
 * onto the line's spline automatically, so you never need to enter a manual
 * spline distance by hand.
 */
UCLASS()
class MILLIONNAIRES_API AMetroStation : public AActor
{
	GENERATED_BODY()

public:
	AMetroStation();

	/** How long (seconds) a train dwells at this station before departing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetroStation")
	float DwellTime = 8.f;

	/** Resolved distance along the owning line's spline. Set by SplineFollowerComponent/the line at BeginPlay. */
	float DistanceAlongSpline = 0.f;

	/** Finds this station's line's UMetroLineComponent, via the Actor this station is attached to
	 *  (stations are placed as children of the line's SplineOwner Actor). Returns nullptr if not
	 *  attached under a line, or if that line has no UMetroLineComponent. */
	UFUNCTION(BlueprintPure, Category = "MetroStation")
	UMetroLineComponent* GetLineComponent() const;

protected:
	UPROPERTY(VisibleAnywhere, Category = "MetroStation")
	TObjectPtr<USceneComponent> Root;
};