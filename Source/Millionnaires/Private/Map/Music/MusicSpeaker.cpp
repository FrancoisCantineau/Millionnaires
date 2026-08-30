// MusicSpeaker.cpp
#include "Map/Music/MusicSpeaker.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundEffectSource.h"
#include "Map/Music/MusicBroadcastSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

namespace
{
	/** Forces spatialization on regardless of whether the assigned Sound asset has its own
	 *  Attenuation settings configured — avoids silently playing 2D/non-spatial audio because a
	 *  content asset forgot to enable "Override Attenuation". */
	void ConfigureSpatialAttenuation(UAudioComponent* Comp, float Radius)
	{
		if (!Comp)
		{
			return;
		}
		Comp->bOverrideAttenuation = true;
		Comp->AttenuationOverrides.bAttenuate = true;
		Comp->AttenuationOverrides.bSpatialize = true;
		Comp->AttenuationOverrides.AttenuationShape = EAttenuationShape::Sphere;
		Comp->AttenuationOverrides.FalloffDistance = Radius;
		Comp->AttenuationOverrides.dBAttenuationAtMax = -60.f;
	}
}

AMusicSpeaker::AMusicSpeaker()
{
	PrimaryActorTick.bCanEverTick = false;

	ActivationVolume = CreateDefaultSubobject<USphereComponent>(TEXT("ActivationVolume"));
	SetRootComponent(ActivationVolume);
	ActivationVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ActivationVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	ActivationVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(RootComponent);
	AudioComp->bAutoActivate = false;

	BroadcastAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("BroadcastAudioComp"));
	BroadcastAudioComp->SetupAttachment(RootComponent);
	BroadcastAudioComp->bAutoActivate = false;

	OneShotAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("OneShotAudioComp"));
	OneShotAudioComp->SetupAttachment(RootComponent);
	OneShotAudioComp->bAutoActivate = false;
}

void AMusicSpeaker::BeginPlay()
{
	Super::BeginPlay();

	UMusicBroadcastSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMusicBroadcastSubsystem>();

	const float EffectiveRadius = (ActivationRadius > 0.f) ? ActivationRadius : (Subsystem ? Subsystem->DefaultActivationRadius : 1500.f);
	ActivationVolume->SetSphereRadius(EffectiveRadius);

	ConfigureSpatialAttenuation(AudioComp, EffectiveRadius);
	ConfigureSpatialAttenuation(BroadcastAudioComp, EffectiveRadius);
	ConfigureSpatialAttenuation(OneShotAudioComp, EffectiveRadius);

	if (RoboticEffectChain)
	{
		AudioComp->SourceEffectChain = RoboticEffectChain;
		BroadcastAudioComp->SourceEffectChain = RoboticEffectChain;
		OneShotAudioComp->SourceEffectChain = RoboticEffectChain;
	}

	ActivationVolume->OnComponentBeginOverlap.AddDynamic(this, &AMusicSpeaker::OnPlayerEnterRange);
	ActivationVolume->OnComponentEndOverlap.AddDynamic(this, &AMusicSpeaker::OnPlayerExitRange);

	if (Subsystem)
	{
		Subsystem->OnBroadcastStateChanged.AddDynamic(this, &AMusicSpeaker::OnBroadcastStateChangedDelegate);
		Subsystem->OnVoiceBroadcastStateChanged.AddDynamic(this, &AMusicSpeaker::OnVoiceBroadcastStateChangedDelegate);
		Subsystem->OnOneShotRequested.AddDynamic(this, &AMusicSpeaker::OnOneShotRequestedDelegate);
	}

	SubscribeToIncidentManager();
}

void AMusicSpeaker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UMusicBroadcastSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMusicBroadcastSubsystem>())
	{
		Subsystem->OnBroadcastStateChanged.RemoveDynamic(this, &AMusicSpeaker::OnBroadcastStateChangedDelegate);
		Subsystem->OnVoiceBroadcastStateChanged.RemoveDynamic(this, &AMusicSpeaker::OnVoiceBroadcastStateChangedDelegate);
		Subsystem->OnOneShotRequested.RemoveDynamic(this, &AMusicSpeaker::OnOneShotRequestedDelegate);
	}
	UnsubscribeFromIncidentManager();

	GetWorld()->GetTimerManager().ClearTimer(FlickerTimerHandle);

	Super::EndPlay(EndPlayReason);
}

void AMusicSpeaker::OnPlayerEnterRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<APawn>(OtherActor))
	{
		return;
	}
	bPlayerInRange = true;
	RefreshMusicPlaybackState();
	RefreshVoicePlaybackState();
}

void AMusicSpeaker::OnPlayerExitRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Cast<APawn>(OtherActor))
	{
		return;
	}
	bPlayerInRange = false;
	RefreshMusicPlaybackState();
	RefreshVoicePlaybackState();
}

// ---------------------------------------------------------------------------
// Music layer
// ---------------------------------------------------------------------------

void AMusicSpeaker::OnBroadcastStateChangedDelegate(bool bIsBroadcasting, USoundBase* Track)
{
	if (bIsBroadcasting && Track && AudioComp->Sound != Track)
	{
		AudioComp->SetSound(Track);
	}
	RefreshMusicPlaybackState();
}

void AMusicSpeaker::RefreshMusicPlaybackState()
{
	UMusicBroadcastSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMusicBroadcastSubsystem>();
	const bool bShouldPlay = Subsystem && Subsystem->IsBroadcasting() && bPlayerInRange && !bPowerCutInMyZone;

	if (bShouldPlay == bIsCurrentlyPlayingMusic)
	{
		return;
	}
	bIsCurrentlyPlayingMusic = bShouldPlay;

	if (bShouldPlay)
	{
		OnMusicStarted(Subsystem ? Subsystem->GetCurrentTrack() : nullptr);
		ScheduleNextFlickerCheck();
	}
	else
	{
		OnMusicStopped();
		GetWorld()->GetTimerManager().ClearTimer(FlickerTimerHandle);
	}
}

void AMusicSpeaker::OnMusicStarted_Implementation(USoundBase* Track)
{
	if (!AudioComp)
	{
		return;
	}

	if (Track && AudioComp->Sound != Track)
	{
		AudioComp->SetSound(Track);
	}

	float StartTime = 0.f;
	if (UMusicBroadcastSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMusicBroadcastSubsystem>())
	{
		StartTime = Subsystem->GetElapsedBroadcastTime();
	}

	AudioComp->Play(StartTime);
	UpdateDuckedVolumes();
}

void AMusicSpeaker::OnMusicStopped_Implementation()
{
	if (AudioComp)
	{
		AudioComp->FadeOut(0.5f, 0.f);
	}
}

void AMusicSpeaker::OnPowerLost_Implementation()
{
	if (AudioComp)
	{
		AudioComp->FadeOut(1.5f, 0.f);
	}
}

void AMusicSpeaker::OnPowerRestored_Implementation()
{
	// RefreshMusicPlaybackState already calls OnMusicStarted (Play + re-sync) when playback
	// actually resumes — nothing more needed by default. Override in Blueprint for an extra
	// "hums back to life" flourish if you want one.
}

void AMusicSpeaker::OnFlicker_Implementation()
{
	if (!AudioComp)
	{
		return;
	}

	// Simple default: a brief volume dip. Override in Blueprint for a static-burst sound instead.
	AudioComp->SetVolumeMultiplier(MusicDuckRequests > 0 ? DuckVolume * 0.3f : 0.3f);

	FTimerHandle TempHandle;
	TWeakObjectPtr<AMusicSpeaker> WeakThis(this);
	GetWorld()->GetTimerManager().SetTimer(TempHandle, [WeakThis]()
	{
		if (WeakThis.IsValid())
		{
			WeakThis->UpdateDuckedVolumes();
		}
	}, 0.3f, false);
}

void AMusicSpeaker::ScheduleNextFlickerCheck()
{
	const float Delay = FMath::Max(FlickerAverageInterval * FMath::FRandRange(0.5f, 1.5f), 1.f);
	GetWorld()->GetTimerManager().SetTimer(FlickerTimerHandle, this, &AMusicSpeaker::CheckFlicker, Delay, false);
}

void AMusicSpeaker::CheckFlicker()
{
	if (!bIsCurrentlyPlayingMusic)
	{
		return;
	}

	if (FMath::FRand() < FlickerChance)
	{
		OnFlicker();
	}

	ScheduleNextFlickerCheck();
}

// ---------------------------------------------------------------------------
// Voice layer
// ---------------------------------------------------------------------------

void AMusicSpeaker::OnVoiceBroadcastStateChangedDelegate(bool bIsActive, USoundBase* Voice, FGameplayTag Zone)
{
	VoiceZoneFilter = Zone;

	if (bIsActive && Voice && BroadcastAudioComp->Sound != Voice)
	{
		BroadcastAudioComp->SetSound(Voice);
	}

	RefreshVoicePlaybackState();
}

void AMusicSpeaker::RefreshVoicePlaybackState()
{
	UMusicBroadcastSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMusicBroadcastSubsystem>();
	const bool bZoneMatches = !VoiceZoneFilter.IsValid() || VoiceZoneFilter == ZoneType;
	const bool bShouldPlay = Subsystem && Subsystem->IsVoiceBroadcasting() && bZoneMatches && bPlayerInRange && !bPowerCutInMyZone;

	if (bShouldPlay == bIsCurrentlyPlayingVoice)
	{
		return;
	}
	bIsCurrentlyPlayingVoice = bShouldPlay;

	if (bShouldPlay)
	{
		++MusicDuckRequests;
		UpdateDuckedVolumes();

		const float StartTime = Subsystem ? Subsystem->GetElapsedVoiceBroadcastTime() : 0.f;
		BroadcastAudioComp->Play(StartTime);
	}
	else
	{
		BroadcastAudioComp->FadeOut(0.3f, 0.f);
		MusicDuckRequests = FMath::Max(MusicDuckRequests - 1, 0);
		UpdateDuckedVolumes();
	}
}

// ---------------------------------------------------------------------------
// One-shot layer
// ---------------------------------------------------------------------------

void AMusicSpeaker::OnOneShotRequestedDelegate(USoundBase* Sound, FGameplayTag Zone)
{
	if (bPowerCutInMyZone)
	{
		return;
	}
	if (Zone.IsValid() && Zone != ZoneType)
	{
		return;
	}

	PlayOneShot(Sound);
}

void AMusicSpeaker::PlayOneShot_Implementation(USoundBase* Sound)
{
	if (!OneShotAudioComp || !Sound)
	{
		return;
	}

	if (OneShotAudioComp->IsPlaying())
	{
		// Queue rather than cutting off the current one — the duck stays active across the
		// whole sequence, only lifting once the queue is fully drained.
		OneShotQueue.Add(Sound);
		return;
	}

	if (!bOneShotSessionActive)
	{
		bOneShotSessionActive = true;
		++MusicDuckRequests;
		++VoiceDuckRequests;
		UpdateDuckedVolumes();
	}

	OneShotAudioComp->SetSound(Sound);
	OneShotAudioComp->OnAudioFinished.AddUniqueDynamic(this, &AMusicSpeaker::OnOneShotFinished);
	OneShotAudioComp->Play();
}

void AMusicSpeaker::OnOneShotFinished()
{
	if (OneShotQueue.Num() > 0)
	{
		USoundBase* Next = OneShotQueue[0];
		OneShotQueue.RemoveAt(0);
		OneShotAudioComp->SetSound(Next);
		OneShotAudioComp->Play();
		return;
	}

	bOneShotSessionActive = false;
	MusicDuckRequests = FMath::Max(MusicDuckRequests - 1, 0);
	VoiceDuckRequests = FMath::Max(VoiceDuckRequests - 1, 0);
	UpdateDuckedVolumes();
}

void AMusicSpeaker::UpdateDuckedVolumes()
{
	if (AudioComp)
	{
		AudioComp->SetVolumeMultiplier(MusicDuckRequests > 0 ? DuckVolume : 1.f);
	}
	if (BroadcastAudioComp)
	{
		BroadcastAudioComp->SetVolumeMultiplier(VoiceDuckRequests > 0 ? DuckVolume : 1.f);
	}
}

// ---------------------------------------------------------------------------
// Incident / power handling
// ---------------------------------------------------------------------------

void AMusicSpeaker::SubscribeToIncidentManager()
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!IsValid(GI))
	{
		return;
	}

	UIncidentManager* IM = GI->GetSubsystem<UIncidentManager>();
	if (!IsValid(IM))
	{
		return;
	}

	IM->OnIncidentTriggered.AddDynamic(this, &AMusicSpeaker::OnIncidentTriggeredDelegate);
	IM->OnIncidentResolved.AddDynamic(this, &AMusicSpeaker::OnIncidentResolvedDelegate);
}

void AMusicSpeaker::UnsubscribeFromIncidentManager()
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!IsValid(GI))
	{
		return;
	}

	UIncidentManager* IM = GI->GetSubsystem<UIncidentManager>();
	if (!IsValid(IM))
	{
		return;
	}

	IM->OnIncidentTriggered.RemoveDynamic(this, &AMusicSpeaker::OnIncidentTriggeredDelegate);
	IM->OnIncidentResolved.RemoveDynamic(this, &AMusicSpeaker::OnIncidentResolvedDelegate);
}

void AMusicSpeaker::OnIncidentTriggeredDelegate(FShipIncident Incident)
{
	// NOTE: reuses "Incident.AffectsSystem.Power" — add this tag if you haven't already.
	if (!Incident.AffectedSystems.HasTag(FGameplayTag::RequestGameplayTag("Incident.AffectsSystem.Power")))
	{
		return;
	}
	if (Incident.AffectedZone.IsValid() && Incident.AffectedZone != ZoneType)
	{
		return;
	}

	bPowerCutInMyZone = true;
	if (bIsCurrentlyPlayingMusic)
	{
		OnPowerLost();
	}
	RefreshMusicPlaybackState();
	RefreshVoicePlaybackState();
}

void AMusicSpeaker::OnIncidentResolvedDelegate(FShipIncident Incident)
{
	if (!Incident.AffectedSystems.HasTag(FGameplayTag::RequestGameplayTag("Incident.AffectsSystem.Power")))
	{
		return;
	}
	if (Incident.AffectedZone.IsValid() && Incident.AffectedZone != ZoneType)
	{
		return;
	}

	const bool bWasCurrentlyPlaying = bIsCurrentlyPlayingMusic;
	bPowerCutInMyZone = false;
	RefreshMusicPlaybackState();
	RefreshVoicePlaybackState();
	if (!bWasCurrentlyPlaying && bIsCurrentlyPlayingMusic)
	{
		OnPowerRestored();
	}
}