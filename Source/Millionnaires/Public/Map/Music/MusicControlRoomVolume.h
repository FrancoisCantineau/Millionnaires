// MusicControlRoomVolume.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MusicControlRoomVolume.generated.h"

class USphereComponent;

/**
 * Place in the control room. Notifies UMusicBroadcastSubsystem when the player enters/exits,
 * gating whether the PA system is allowed to broadcast at all.
 */
UCLASS()
class MILLIONNAIRES_API AMusicControlRoomVolume : public AActor
{
	GENERATED_BODY()

public:
	AMusicControlRoomVolume();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, Category = "MusicControlRoom")
	TObjectPtr<USphereComponent> PresenceVolume;

	/** How many relevant Pawns are currently inside — supports more than one without the state flickering off early. */
	int32 OccupantCount = 0;
};
