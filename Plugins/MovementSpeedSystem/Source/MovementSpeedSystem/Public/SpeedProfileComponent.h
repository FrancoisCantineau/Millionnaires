// SpeedProfileComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "MovementSpeedConfig.h"
#include "SpeedProfileComponent.generated.h"

/**
 * Generic named-speed component — add to the player OR any enemy type. Points at a
 * MovementSpeedConfig DataAsset for its values, so multiple actors sharing the same archetype
 * reference the SAME asset and get identical values tuned in one place.
 *
 * Two independent concepts:
 * - SpeedProfile: the EXCLUSIVE current intent (Patrol / Chase / Flee) — only one at a time.
 * - Speed modifiers: COMBINABLE constraints (Injured, Mud, Slowed) that can stack on top of
 *   whichever profile is active. Final speed = ProfileSpeed x all active multipliers, then
 *   clamped to the lowest active cap.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MOVEMENTSPEEDSYSTEM_API USpeedProfileComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpeedProfileComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	TObjectPtr<UMovementSpeedConfig> SpeedConfig;

	/** Sets the exclusive base profile (Patrol/Chase/Flee/...). Returns false if SpeedConfig is
	 *  unset, the tag isn't found in SpeedProfiles, or the owner isn't a Character. */
	UFUNCTION(BlueprintCallable, Category = "Speed")
	bool SetSpeedProfile(FGameplayTag ProfileTag);

	/** Adds a combinable modifier (e.g. Status.Injured, Surface.Mud) — multiple can be active
	 *  at once, stacking with the current profile. */
	UFUNCTION(BlueprintCallable, Category = "Speed")
	void AddSpeedModifier(FGameplayTag ModifierTag);

	UFUNCTION(BlueprintCallable, Category = "Speed")
	void RemoveSpeedModifier(FGameplayTag ModifierTag);

	UFUNCTION(BlueprintPure, Category = "Speed")
	FGameplayTag GetCurrentProfile() const { return CurrentProfile; }

private:
	UPROPERTY(Transient)
	FGameplayTag CurrentProfile;

	UPROPERTY(Transient)
	TSet<FGameplayTag> ActiveModifiers;

	/** Recomputes ProfileSpeed x multipliers, clamped to the lowest active cap, and applies it. */
	void RecalculateSpeed();
};
