// MusicControlRoomVolume.cpp
#include "MusicControlRoomVolume.h"
#include "Components/SphereComponent.h"
#include "MusicBroadcastSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

AMusicControlRoomVolume::AMusicControlRoomVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	PresenceVolume = CreateDefaultSubobject<USphereComponent>(TEXT("PresenceVolume"));
	SetRootComponent(PresenceVolume);
	PresenceVolume->SetSphereRadius(300.f);
	PresenceVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PresenceVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	PresenceVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMusicControlRoomVolume::BeginPlay()
{
	Super::BeginPlay();

	PresenceVolume->OnComponentBeginOverlap.AddDynamic(this, &AMusicControlRoomVolume::OnOverlapBegin);
	PresenceVolume->OnComponentEndOverlap.AddDynamic(this, &AMusicControlRoomVolume::OnOverlapEnd);
}

void AMusicControlRoomVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<APawn>(OtherActor))
	{
		return;
	}

	++OccupantCount;
	if (OccupantCount == 1)
	{
		if (UMusicBroadcastSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMusicBroadcastSubsystem>())
		{
			Subsystem->SetControlRoomOccupied(true);
		}
	}
}

void AMusicControlRoomVolume::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Cast<APawn>(OtherActor))
	{
		return;
	}

	OccupantCount = FMath::Max(OccupantCount - 1, 0);
	if (OccupantCount == 0)
	{
		if (UMusicBroadcastSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMusicBroadcastSubsystem>())
		{
			Subsystem->SetControlRoomOccupied(false);
		}
	}
}
