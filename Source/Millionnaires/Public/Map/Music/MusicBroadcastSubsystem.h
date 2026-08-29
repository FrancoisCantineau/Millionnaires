// MusicBroadcastSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MusicBroadcastSubsystem.generated.h"

class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBroadcastStateChanged, bool, bIsBroadcasting, USoundBase*, Track);

/**
 * Tracks whether the control-room PA system should currently be broadcasting music.
 * Broadcasting is only actually active when BOTH music has been requested (e.g. by asking the
 * control-room NPC) AND someone is physically present in the control room — leave the room and
 * the music stops, regardless of the request state.
 *
 * AMusicSpeaker instances subscribe to OnBroadcastStateChanged to know when to start/stop.
 */
UCLASS()
class MILLIONNAIRES_API UMusicBroadcastSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
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

	/** Fired whenever the EFFECTIVE broadcasting state (request AND occupancy combined) changes. */
	UPROPERTY(BlueprintAssignable, Category = "MusicBroadcast")
	FOnBroadcastStateChanged OnBroadcastStateChanged;

private:
	bool bMusicRequested = false;
	bool bControlRoomOccupied = false;

	UPROPERTY(Transient)
	TObjectPtr<USoundBase> CurrentTrack;

	/** Recomputes IsBroadcasting() and fires OnBroadcastStateChanged only if it actually changed. */
	void RefreshBroadcastState();

	bool bLastBroadcastingState = false;
};
