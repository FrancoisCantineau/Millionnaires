// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/GameplayAction_PlaySound.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Core/GameplayEventContext.h"
 
void UGameplayAction_PlaySound::Execute_Implementation(const FEventContext& Context)
{
	if (!Sound)
	{
		return;
	}
 
	if (bAtSenderLocation)
	{
		// No sender loaded/valid = nobody around to hear it. Silently skip, no WorldState fallback needed.
		if (!Context.Sender.IsValid())
		{
			return;
		}
		UGameplayStatics::PlaySoundAtLocation(Context.Sender.Get(), Sound, Context.Sender->GetActorLocation(), VolumeMultiplier, PitchMultiplier);
	}
	else
	{
		UWorld* World = Context.Sender.IsValid() ? Context.Sender->GetWorld() : (GEngine ? GEngine->GetCurrentPlayWorld() : nullptr);
		if (!World)
		{
			return;
		}
		UGameplayStatics::PlaySound2D(World, Sound, VolumeMultiplier, PitchMultiplier);
	}
}
