#pragma once

#include "CoreMinimal.h"
#include "SaveableActorReference.generated.h"

/**
 * Designer-facing way to reference a saveable actor from outside it — e.g.
 * from an event orchestrator step ("move THIS door"). In the editor, drag
 * any actor into TargetActor; this works even for an actor in a level that
 * isn't currently loaded, since TSoftObjectPtr only stores a path.
 *
 * The reference is resolved ONCE, in the editor (RefreshCachedId), into a
 * stable FGuid. At runtime, systems should read ONLY CachedId — never
 * TargetActor — so referencing an object never forces it to load.
 */
USTRUCT(BlueprintType)
struct SAVEFRAMEWORK_API FSaveableActorReference
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SaveFramework")
	TSoftObjectPtr<AActor> TargetActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveFramework")
	FGuid CachedId;

	bool IsValid() const { return CachedId.IsValid(); }

#if WITH_EDITOR
	/**
	 * Call this whenever TargetActor changes — e.g. from PostEditChangeProperty
	 * on the object that owns this struct (an orchestrator step DataAsset,
	 * typically). Not automated yet: a proper details-panel customization
	 * that calls this transparently belongs in the (currently empty)
	 * Editor/ folder, once the orchestrator actually needs it.
	 */
	void RefreshCachedId();
#endif
};
