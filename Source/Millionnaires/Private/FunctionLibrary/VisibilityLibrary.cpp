// VisibilityLibrary.cpp
#include "FunctionLibrary/VisibilityLibrary.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"

bool UVisibilityLibrary::CanAnyWatcherSeeActor(const TArray<AActor*>& Watchers, AActor* Target)
{
	for (AActor* Watcher : Watchers)
	{
		if (CanActorSeeActor(Watcher, Target))
		{
			return true; // one witness is enough
		}
	}
	return false;
}

bool UVisibilityLibrary::CanActorSeeActor(AActor* Watcher, AActor* Target)
{
	if (!Watcher || !Target || !Watcher->GetWorld())
	{
		return false;
	}

	FVector EyeLocation = Watcher->GetActorLocation();
	FRotator EyeRotation;
	if (const APawn* WatcherPawn = Cast<APawn>(Watcher))
	{
		WatcherPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	}

	const FVector TargetLocation = Target->GetActorLocation();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Watcher);
	QueryParams.AddIgnoredActor(Target);

	FHitResult HitResult;
	const bool bBlocked = Watcher->GetWorld()->LineTraceSingleByChannel(
		HitResult,
		EyeLocation,
		TargetLocation,
		ECC_Visibility,
		QueryParams
	);

	// If something blocks the line before reaching Target, Target isn't visible.
	return !bBlocked;
}