// MusicBroadcastSubsystem.cpp
#include "MusicBroadcastSubsystem.h"

void UMusicBroadcastSubsystem::RequestMusic(bool bStart, USoundBase* Track)
{
	bMusicRequested = bStart;
	if (bStart && Track)
	{
		CurrentTrack = Track;
	}
	RefreshBroadcastState();
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
		OnBroadcastStateChanged.Broadcast(bNowBroadcasting, CurrentTrack);
	}
}
