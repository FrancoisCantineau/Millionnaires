// MusicBroadcastSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "MusicBroadcastSubsystem.generated.h"

class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBroadcastStateChanged, bool, bIsBroadcasting, USoundBase*, Track);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnVoiceBroadcastStateChanged, bool, bIsActive, USoundBase*, Voice, FGameplayTag, Zone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOneShotRequested, USoundBase*, Sound, FGameplayTag, Zone);

/**
 * Tracks whether the control-room PA system should currently be broadcasting music.
 * Broadcasting is only actually active when BOTH music has been requested (e.g. by asking the
 * control-room NPC) AND someone is physically present in the control room — leave the room and
 * the music stops, regardless of the request state.
 *
 * AMusicSpeaker instances subscribe to OnBroadcastStateChanged to know when to start/stop.
 */
UCLASS(Config = Game)
class MILLIONNAIRES_API UMusicBroadcastSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Default speaker activation radius (cm), used by any AMusicSpeaker whose own ActivationRadius
	 *  is left at -1 (the "use default" sentinel). Editable via DefaultGame.ini under
	 *  [/Script/Millionnaires.MusicBroadcastSubsystem], or at runtime via SetDefaultActivationRadius. */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category = "MusicBroadcast")
	float DefaultActivationRadius = 1500.f;
	/** Call when the player asks the control-room NPC to start/stop music (wire this up to your dialogue system later). */
	UFUNCTION(BlueprintCallable, Category = "MusicBroadcast")
	void RequestMusic(bool bStart, USoundBase* Track = nullptr);

	/** Call from a control-room presence trigger when someone enters/exits. */
	UFUNCTION(BlueprintCallable, Category = "MusicBroadcast")
	void SetControlRoomOccupied(bool bOccupied);

	UFUNCTION(BlueprintPure, Category = "MusicBroadcast")
	bool IsBroadcasting() const { return bMusicRequested && bControlRoomOccupied; }

	UFUNCTION(BlueprintPure, Category = "MusicBroadcast")
	USoundBase* GetCurrentTrack() const { return CurrentTrack; }

	/** Seconds elapsed since the current broadcast started — use this as the Play() start time
	 *  so a speaker joining mid-track (player walks into range later) stays in sync with the others. */
	UFUNCTION(BlueprintPure, Category = "MusicBroadcast")
	float GetElapsedBroadcastTime() const;

	/** Fired whenever the EFFECTIVE broadcasting state (request AND occupancy combined) changes. */
	UPROPERTY(BlueprintAssignable, Category = "MusicBroadcast")
	FOnBroadcastStateChanged OnBroadcastStateChanged;

	/** Plays Sound once on every speaker whose ZoneType matches Zone (or every speaker if Zone is
	 *  invalid). Completely generic — a voice line, an SFX, an alarm, whatever. Ducks that
	 *  speaker's Music and Voice layers for the sound's duration, then restores them.
	 *  For a SINGLE specific speaker instead of a zone, skip this and call
	 *  Speaker->PlayOneShot(Sound) directly on its reference instead. */
	UFUNCTION(BlueprintCallable, Category = "MusicBroadcast")
	void PlayOneShot(USoundBase* Sound, FGameplayTag Zone);

	/** Fired by PlayOneShot — each AMusicSpeaker subscribes and decides for itself whether Zone applies to it. */
	UPROPERTY(BlueprintAssignable, Category = "MusicBroadcast")
	FOnOneShotRequested OnOneShotRequested;

	/** Like RequestMusic, but for an ongoing/live voice broadcast (someone talking over the PA,
	 *  a haunting voice, etc.) that follows the player across speakers the same way music does —
	 *  and ducks music while active. No occupancy gate by default (unlike music) — layer one on
	 *  the calling side if you want one. Zone scopes which speakers carry it (invalid = everywhere). */
	UFUNCTION(BlueprintCallable, Category = "MusicBroadcast")
	void RequestVoiceBroadcast(bool bStart, USoundBase* Voice, FGameplayTag Zone);

	UFUNCTION(BlueprintPure, Category = "MusicBroadcast")
	bool IsVoiceBroadcasting() const { return bVoiceRequested; }

	/** Seconds elapsed since the current voice broadcast started — same sync purpose as GetElapsedBroadcastTime. */
	UFUNCTION(BlueprintPure, Category = "MusicBroadcast")
	float GetElapsedVoiceBroadcastTime() const;

	UPROPERTY(BlueprintAssignable, Category = "MusicBroadcast")
	FOnVoiceBroadcastStateChanged OnVoiceBroadcastStateChanged;

private:
	bool bMusicRequested = false;
	bool bControlRoomOccupied = false;

	UPROPERTY(Transient)
	TObjectPtr<USoundBase> CurrentTrack;

	/** Recomputes IsBroadcasting() and fires OnBroadcastStateChanged only if it actually changed. */
	void RefreshBroadcastState();

	bool bLastBroadcastingState = false;
	float BroadcastStartWorldTime = 0.f;

	bool bVoiceRequested = false;
	FGameplayTag VoiceZone;
	UPROPERTY(Transient)
	TObjectPtr<USoundBase> CurrentVoice;
	bool bLastVoiceBroadcastingState = false;
	float VoiceBroadcastStartWorldTime = 0.f;
};