// VisibilityLibrary.h
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VisibilityLibrary.generated.h"

UCLASS()
class MILLIONNAIRES_API UVisibilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** True if ANY of Watchers currently has an unobstructed line of sight to Target — one
	 *  witness is enough to count as "seen". Solo V0 callers just pass a single-element array
	 *  (e.g. { TargetActor }); a future multiplayer caller passes every player's Pawn instead —
	 *  this function's signature and logic never need to change either way. */
	UFUNCTION(BlueprintCallable, Category = "Visibility")
	static bool CanAnyWatcherSeeActor(const TArray<AActor*>& Watchers, AActor* Target);

private:
	static bool CanActorSeeActor(AActor* Watcher, AActor* Target);
};