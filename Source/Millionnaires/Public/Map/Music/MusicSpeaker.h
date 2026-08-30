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
class USoundEffectSourcePresetChain;

/**
 * A physical speaker with three independent, layered audio channels, each on its own
 * AudioComponent, in increasing priority:
 *
 *  - Music     (AudioComp)          — looping background music, follows the player like the
 *                                     others, gated by UMusicBroadcastSubsystem::IsBroadcasting().
 *  - Voice     (BroadcastAudioComp) — an ongoing/live voice broadcast (PA announcer, a haunting
 *                                     voice, etc.) that also follows the player across speakers,
 *                                     and ducks Music while active.
 *  - OneShot   (OneShotAudioComp)   — a single generic sound (voice line, SFX, alarm...), ducks
 *                                     BOTH Music and Voice while it plays, then restores them.
 *                                     Overlapping one-shots queue rather than cutting each other off.
 *
 * All three channels only ever play while the player is within ActivationRadius of THIS speaker
 * and this speaker's zone doesn't have an active power-affecting incident — Music and Voice
 * additionally require their respective subsystem-level broadcast to be active.
 *
 * Addressing a speaker: hold a direct reference and call PlayOneShot()/etc. on it for a SINGLE
 * speaker; use the subsystem's zone-scoped functions (PlayOneShot(Sound, Zone), RequestVoiceBroadcast
 * with a Zone) for a whole zone; leave Zone invalid on those for every speaker on the map.
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
	void OnVoiceBroadcastStateChangedDelegate(bool bIsActive, USoundBase* Voice, FGameplayTag Zone);

	UFUNCTION()
	void OnOneShotRequestedDelegate(USoundBase* Sound, FGameplayTag Zone);

	UFUNCTION()
	void OnOneShotFinished();

	UFUNCTION()
	void OnIncidentTriggeredDelegate(FShipIncident Incident);

	UFUNCTION()
	void OnIncidentResolvedDelegate(FShipIncident Incident);

public:
	/** Music/background layer. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MusicSpeaker")
	TObjectPtr<UAudioComponent> AudioComp;

	/** Voice/PA layer — ducks AudioComp while active. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MusicSpeaker")
	TObjectPtr<UAudioComponent> BroadcastAudioComp;

	/** One-shot layer — ducks both of the above while playing. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MusicSpeaker")
	TObjectPtr<UAudioComponent> OneShotAudioComp;

	/** How close the player needs to be for this speaker to turn on. Leave at -1 to use
	 *  UMusicBroadcastSubsystem::DefaultActivationRadius instead of setting one per speaker. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicSpeaker")
	float ActivationRadius = -1.f;

	/** Identifies this speaker's electrical zone — used for both incident matching and
	 *  zone-scoped Voice/OneShot targeting. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicSpeaker")
	FGameplayTag ZoneType;

	/** Volume multiplier applied to a layer while something higher-priority is ducking it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicSpeaker")
	float DuckVolume = 0.2f;

	/** Optional Source Effect Chain (Bit Crusher, Ring Modulator, etc.) applied to everything
	 *  this speaker plays, for a degraded/robotic PA tone. Create the chain asset in the Content
	 *  Browser (right-click > Sounds > Source Effects) and assign it here — left empty, speakers
	 *  play with no extra processing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicSpeaker")
	TObjectPtr<USoundEffectSourcePresetChain> RoboticEffectChain;

	/** Roughly how often (seconds, average) a flicker/dropout can occur while Music is playing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicSpeaker|Flicker")
	float FlickerAverageInterval = 20.f;

	/** Chance [0-1] a flicker actually happens when its roll comes up, vs. being skipped. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicSpeaker|Flicker")
	float FlickerChance = 0.3f;

	/** Plays Sound once on THIS speaker specifically, queueing if another one-shot is already
	 *  playing here. Default C++ behavior: ducks Music+Voice, plays, restores them when done
	 *  (including through a queue of several). Override in Blueprint only for custom behavior. */
	UFUNCTION(BlueprintNativeEvent, Category = "MusicSpeaker")
	void PlayOneShot(USoundBase* Sound);
	virtual void PlayOneShot_Implementation(USoundBase* Sound);

	/** Fired when Music starts/stops on this speaker. Default C++ behavior already plays/fades
	 *  AudioComp — these are for an extra Blueprint flourish (lights, VFX) if you want one. */
	UFUNCTION(BlueprintNativeEvent, Category = "MusicSpeaker")
	void OnMusicStarted(USoundBase* Track);
	virtual void OnMusicStarted_Implementation(USoundBase* Track);

	UFUNCTION(BlueprintNativeEvent, Category = "MusicSpeaker")
	void OnMusicStopped();
	virtual void OnMusicStopped_Implementation();

	/** Fired when this speaker's zone loses power while Music was playing. Default: fade-out.
	 *  Override in Blueprint for a pitch-slowdown "dying down" effect if you want one. */
	UFUNCTION(BlueprintNativeEvent, Category = "MusicSpeaker")
	void OnPowerLost();
	virtual void OnPowerLost_Implementation();

	/** Fired when power is restored and Music actually resumes. Default: no-op (OnMusicStarted
	 *  already handles the resume) — this is only for an extra flourish. */
	UFUNCTION(BlueprintNativeEvent, Category = "MusicSpeaker")
	void OnPowerRestored();
	virtual void OnPowerRestored_Implementation();

	/** Fired occasionally while Music is playing. Default: a brief volume dip. */
	UFUNCTION(BlueprintNativeEvent, Category = "MusicSpeaker")
	void OnFlicker();
	virtual void OnFlicker_Implementation();

private:
	UPROPERTY(VisibleAnywhere, Category = "MusicSpeaker")
	TObjectPtr<USphereComponent> ActivationVolume;

	bool bPlayerInRange = false;
	bool bPowerCutInMyZone = false;

	bool bIsCurrentlyPlayingMusic = false;
	bool bIsCurrentlyPlayingVoice = false;

	FGameplayTag VoiceZoneFilter;

	/** How many active layers currently want Music ducked (Voice starting, OneShot playing, etc). */
	int32 MusicDuckRequests = 0;
	/** How many active layers currently want Voice ducked (OneShot playing). */
	int32 VoiceDuckRequests = 0;

	TArray<TObjectPtr<USoundBase>> OneShotQueue;
	bool bOneShotSessionActive = false;

	FTimerHandle FlickerTimerHandle;

	void RefreshMusicPlaybackState();
	void RefreshVoicePlaybackState();
	void UpdateDuckedVolumes();

	void ScheduleNextFlickerCheck();
	void CheckFlicker();

	void SubscribeToIncidentManager();
	void UnsubscribeFromIncidentManager();
};