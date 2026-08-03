#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SaveGuidComponent.generated.h"

/**
 * Drop this on any actor to give it a stable identity (GUID) across
 * sessions AND generic persistence behaviour (position, active/visible)
 * handled automatically — no custom code required.
 *
 * For a placed actor, the GUID is generated once, in the editor, and
 * stored with the level. Actors spawned dynamically at runtime need a
 * different identity strategy (a "spawn record"), not covered yet.
 *
 * Custom state (HP, inventory, quest flags...) is a separate concern,
 * handled by ISaveable on the actor itself if it needs it. This
 * component only ever deals with the generic, universal part.
 */
UCLASS(ClassGroup = (SaveFramework), meta = (BlueprintSpawnableComponent))
class SAVEFRAMEWORK_API USaveGuidComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USaveGuidComponent();

	UFUNCTION(BlueprintPure, Category = "SaveFramework")
	FGuid GetSaveId() const { return SaveId; }

	/** Moves the owner. Called either immediately by USaveFrameworkWorldState (actor already loaded), or by this component itself on load if a move was pending. */
	void ApplyTransform(const FTransform& NewTransform);

	/** Toggles owner visibility + collision. Same dual call path as ApplyTransform. */
	void ApplyActive(bool bNewActive);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveFramework")
	FGuid SaveId;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	virtual void OnRegister() override;
#endif
};
