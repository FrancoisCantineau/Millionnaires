// MusicBroadcastSubsystem.cpp
#include "Map/Music/MusicBroadcastSubsystem.h"

void UMusicBroadcastSubsystem::RequestMusic(bool bStart, USoundBase* Track)
{
	bMusicRequested = bStart;
	if (bStart && Track)
	{
		CurrentTrack = Track;
	}
	RefreshBroadcastState();
}

void UMusicBroadcastSubsystem::PlayOneShot(USoundBase* Sound, FGameplayTag Zone)
{
	if (Sound)
	{
		OnOneShotRequested.Broadcast(Sound, Zone);
	}
}

void UMusicBroadcastSubsystem::RequestVoiceBroadcast(bool bStart, USoundBase* Voice, FGameplayTag Zone)
{
	bVoiceRequested = bStart;
	VoiceZone = Zone;
	if (bStart && Voice)
	{
		CurrentVoice = Voice;
	}

	if (bVoiceRequested != bLastVoiceBroadcastingState)
	{
		bLastVoiceBroadcastingState = bVoiceRequested;

		if (bVoiceRequested)
		{
			if (UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr)
			{
				VoiceBroadcastStartWorldTime = World->GetTimeSeconds();
			}
		}

		OnVoiceBroadcastStateChanged.Broadcast(bVoiceRequested, CurrentVoice, VoiceZone);
	}
}

float UMusicBroadcastSubsystem::GetElapsedVoiceBroadcastTime() const
{
	if (!bLastVoiceBroadcastingState)
	{
		return 0.f;
	}

	UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	return World ? (World->GetTimeSeconds() - VoiceBroadcastStartWorldTime) : 0.f;
}

void UMusicBroadcastSubsystem::SetControlRoomOccupied(bool bOccupied)
{
	bControlRoomOccupied = bOccupied;
	RefreshBroadcastState();
}

void UMusicBroadcastSubsystem::RefreshBroadcastState()
{
	const bool bNowBroadcasting = IsBroadcasting();
	if (bNowBroadcasting != bLastBroadcastingState)
	{
		bLastBroadcastingState = bNowBroadcasting;

		if (bNowBroadcasting)
		{
			if (UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr)
			{
				BroadcastStartWorldTime = World->GetTimeSeconds();
			}
		}

		OnBroadcastStateChanged.Broadcast(bNowBroadcasting, CurrentTrack);
	}
}

float UMusicBroadcastSubsystem::GetElapsedBroadcastTime() const
{
	if (!bLastBroadcastingState)
	{
		return 0.f;
	}

	UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	return World ? (World->GetTimeSeconds() - BroadcastStartWorldTime) : 0.f;
}