// MovementSpeedConfig.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MovementSpeedConfig.generated.h"

/**
 * Shareable speed configuration — create one per "archetype" that needs distinct values (e.g.
 * DA_HorrorEnemySpeed, DA_FastEnemySpeed), not one per tag. Multiple enemy Blueprints can
 * reference the SAME asset to share identical values and tune them in one place.
 */
UCLASS(BlueprintType)
class MOVEMENTSPEEDSYSTEM_API UMovementSpeedConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Speed|Profiles")
	TMap<FGameplayTag, float> SpeedProfiles;

	/** Caps active while their tag is applied — final speed is clamped to the LOWEST active cap. */
	UPROPERTY(EditAnywhere, Category = "Speed|Caps")
	TMap<FGameplayTag, float> SpeedCaps;

	/** Multipliers active while their tag is applied — stack multiplicatively (e.g. Mud x0.8 AND
	 *  Slowed x0.6 together = x0.48). */
	UPROPERTY(EditAnywhere, Category = "Speed|Multipliers")
	TMap<FGameplayTag, float> SpeedMultipliers;
};
