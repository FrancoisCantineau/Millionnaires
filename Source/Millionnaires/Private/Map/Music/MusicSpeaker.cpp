// MusicSpeaker.cpp
#include "MusicSpeaker.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "MusicBroadcastSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

AMusicSpeaker::AMusicSpeaker()
{
	PrimaryActorTick.bCanEverTick = false;

	ActivationVolume = CreateDefaultSubobject<USphereComponent>(TEXT("ActivationVolume"));
	SetRootComponent(ActivationVolume);
	ActivationVolume->SetSphereRadius(ActivationRadius);
	ActivationVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ActivationVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	ActivationVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(RootComponent);
	AudioComp->bAutoActivate = false;
}

void AMusicSpeaker::BeginPlay()
{
	Super::BeginPlay();

	ActivationVolume->SetSphereRadius(ActivationRadius);
	ActivationVolume->OnComponentBeginOverlap.AddDynamic(this, &AMusicSpeaker::OnPlayerEnterRange);
	ActivationVolume->OnComponentEndOverlap.AddDynamic(this, &AMusicSpeaker::OnPlayerExitRange);

	if (UMusicBroadcastSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMusicBroadcastSubsystem>())
	{
		Subsystem->OnBroadcastStateChanged.AddDynamic(this, &AMusicSpeaker::OnBroadcastStateChangedDelegate);
	}

	SubscribeToIncidentManager();
}

void AMusicSpeaker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UMusicBroadcastSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMusicBroadcastSubsystem>())
	{
		Subsystem->OnBroadcastStateChanged.RemoveDynamic(this, &AMusicSpeaker::OnBroadcastStateChangedDelegate);
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
	RefreshPlaybackState();
}

void AMusicSpeaker::OnPlayerExitRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Cast<APawn>(OtherActor))
	{
		return;
	}
	bPlayerInRange = false;
	RefreshPlaybackState();
}

void AMusicSpeaker::OnBroadcastStateChangedDelegate(bool bIsBroadcasting, USoundBase* Track)
{
	if (bIsBroadcasting && Track && AudioComp->Sound != Track)
	{
		AudioComp->SetSound(Track);
	}
	RefreshPlaybackState();
}

void AMusicSpeaker::RefreshPlaybackState()
{
	UMusicBroadcastSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMusicBroadcastSubsystem>();
	const bool bShouldPlay = Subsystem && Subsystem->IsBroadcasting() && bPlayerInRange && !bPowerCutInMyZone;

	if (bShouldPlay == bIsCurrentlyPlaying)
	{
		return;
	}
	bIsCurrentlyPlaying = bShouldPlay;

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

void AMusicSpeaker::ScheduleNextFlickerCheck()
{
	const float Delay = FMath::Max(FlickerAverageInterval * FMath::FRandRange(0.5f, 1.5f), 1.f);
	GetWorld()->GetTimerManager().SetTimer(FlickerTimerHandle, this, &AMusicSpeaker::CheckFlicker, Delay, false);
}

void AMusicSpeaker::CheckFlicker()
{
	if (!bIsCurrentlyPlaying)
	{
		return;
	}

	if (FMath::FRand() < FlickerChance)
	{
		OnFlicker();
	}

	ScheduleNextFlickerCheck();
}

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
	// NOTE: reuses "Incident.AffectsSystem.Power" — add this tag if you haven't already
	// (see the note in UMetroLineComponent about "Incident.AffectsSystem.Transit").
	if (!Incident.AffectedSystems.HasTag(FGameplayTag::RequestGameplayTag("Incident.AffectsSystem.Power")))
	{
		return;
	}
	if (Incident.AffectedZone.IsValid() && Incident.AffectedZone != ZoneType)
	{
		return;
	}

	bPowerCutInMyZone = true;
	if (bIsCurrentlyPlaying)
	{
		OnPowerLost();
	}
	RefreshPlaybackState();
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

	const bool bWasCurrentlyPlaying = bIsCurrentlyPlaying;
	bPowerCutInMyZone = false;
	RefreshPlaybackState();
	if (!bWasCurrentlyPlaying && bIsCurrentlyPlaying)
	{
		OnPowerRestored();
	}
}
