// MusicSpeaker.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Map/Events/IncidentManager.h"
#include "MusicSpeaker.generated.h"

class USphereComponent;
class UAudioComponent;
class USoundBase;

/**
 * A physical speaker. Only actually plays when ALL of these are true:
 *  - The control-room PA is broadcasting (UMusicBroadcastSubsystem::IsBroadcasting())
 *  - The player is within ActivationRadius of this speaker (proximity, not a logical zone —
 *    this exists purely so not every speaker on the map plays at once)
 *  - This speaker's zone doesn't currently have an active power-affecting incident
 *
 * The actual fade/pitch-slowdown/muffled-speaker-filter effects are NOT implemented here —
 * OnMusicStarted/OnMusicStopped/OnPowerLost/OnPowerRestored/OnFlicker are Blueprint events for
 * you to drive with Timelines (fade curves, a LowPassFilterFrequency lerp for the speaker's
 * muffled tone, a PitchMultiplier lerp for the "dying down" effect on power loss).
 */
UCLASS()
class MILLIONNAIRES_API AMusicSpeaker : public AActor
{
	GENERATED_BODY()

public:
	AMusicSpeaker();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnPlayerEnterRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerExitRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnBroadcastStateChangedDelegate(bool bIsBroadcasting, USoundBase* Track);

	UFUNCTION()
	void OnIncidentTriggeredDelegate(FShipIncident Incident);

	UFUNCTION()
	void OnIncidentResolvedDelegate(FShipIncident Incident);

public:
	/** The AudioComponent actually playing the track — use this directly in Blueprint for
	 *  FadeIn/FadeOut/SetPitchMultiplier/SetLowPassFilterEnabled calls. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MusicSpeaker")
	TObjectPtr<UAudioComponent> AudioComp;

	/** How close the player needs to be for this speaker to turn on at all. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicSpeaker")
	float ActivationRadius = 1500.f;

	/** Identifies this speaker's electrical zone, for incident matching (mirrors ULightBaseComponent's ZoneType). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicSpeaker")
	FGameplayTag ZoneType;

	/** Roughly how often (seconds, average) a flicker/dropout can occur while playing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicSpeaker|Flicker")
	float FlickerAverageInterval = 20.f;

	/** Chance [0-1] a flicker actually happens when its roll comes up, vs. being skipped. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicSpeaker|Flicker")
	float FlickerChance = 0.3f;

	/** Fired when this speaker should start playing (all conditions met). Play AudioComp here with your fade-in/LPF setup. */
	UFUNCTION(BlueprintImplementableEvent, Category = "MusicSpeaker")
	void OnMusicStarted(USoundBase* Track);

	/** Fired when this speaker should stop (any condition became false). */
	UFUNCTION(BlueprintImplementableEvent, Category = "MusicSpeaker")
	void OnMusicStopped();

	/** Fired when this speaker's zone loses power while it was playing — implement the fade-out + slowdown effect. */
	UFUNCTION(BlueprintImplementableEvent, Category = "MusicSpeaker")
	void OnPowerLost();

	/** Fired when power is restored — only actually resume playback if still in-range and broadcasting. */
	UFUNCTION(BlueprintImplementableEvent, Category = "MusicSpeaker")
	void OnPowerRestored();

	/** Fired occasionally while playing, for a brief volume dip / static burst — purely cosmetic. */
	UFUNCTION(BlueprintImplementableEvent, Category = "MusicSpeaker")
	void OnFlicker();

private:
	UPROPERTY(VisibleAnywhere, Category = "MusicSpeaker")
	TObjectPtr<USphereComponent> ActivationVolume;

	bool bPlayerInRange = false;
	bool bPowerCutInMyZone = false;

	FTimerHandle FlickerTimerHandle;

	/** Re-evaluates whether this speaker should be playing right now and fires the appropriate event on change. */
	void RefreshPlaybackState();

	bool bIsCurrentlyPlaying = false;

	void ScheduleNextFlickerCheck();
	void CheckFlicker();

	void SubscribeToIncidentManager();
	void UnsubscribeFromIncidentManager();
};
