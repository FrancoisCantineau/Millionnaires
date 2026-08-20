#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayEventEmitterComponent.generated.h"

/**
 * Drop this on any actor that needs to publish events on the GameplayEventBus, without
 * writing the GetGameInstance()->GetSubsystem<UGameplayEventBus>() boilerplate by hand
 * every time (this is exactly what ARepairableEquipmentActor used to do inline).
 *
 * Fully generic - has no idea who's listening (a quest objective, an Orchestrator
 * sequence, an achievement tracker, nobody at all).
 */
UCLASS(ClassGroup = (GameplayCore), meta = (BlueprintSpawnableComponent))
class GAMEPLAYCORE_API UGameplayEventEmitterComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	/** Default tag used by Publish(). Leave unset if this component only ever publishes via PublishTag(). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Core|Event Emitter")
	FGameplayTag DefaultEventTag;

	/** Publishes DefaultEventTag, with the owning actor as Sender. */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Core|Event Emitter")
	void Publish();

	/** Publishes an arbitrary tag (ignores DefaultEventTag), still with the owning actor as Sender.
	 *  Useful for one actor that needs to announce several distinct things. */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Core|Event Emitter")
	void PublishTag(FGameplayTag EventTag);

	/** Same as PublishTag, but lets you set a different Instigator than the owning actor
	 *  (e.g. the player who triggered this, if that's not the actor itself). */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Core|Event Emitter")
	void PublishTagWithInstigator(FGameplayTag EventTag, AActor* Instigator);
};
