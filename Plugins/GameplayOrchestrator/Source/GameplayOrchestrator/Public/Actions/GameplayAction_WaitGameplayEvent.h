#pragma once

#include "CoreMinimal.h"
#include "GameplayActionBase.h"
#include "GameplayTagContainer.h"
#include "GameplayAction_WaitGameplayEvent.generated.h"

/**
 * Wait for a specific tag being published on the bus, before considering the step over.
 * For exemple, "Open Door" (Instant, will only launch the anim), followed by a step "Wait Event : Event.Door.Opened",
 * rather than guessing the delay for the anim execution.
 */
UCLASS(meta = (DisplayName = "Wait Gameplay Event"))
class GAMEPLAYORCHESTRATOR_API UGameplayAction_WaitGameplayEvent : public UGameplayActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wait Gameplay Event")
	FGameplayTag EventTagToWaitFor;

	/**
	 * If true, only accept its sender's event (the one that launched the current sequence). Most of the time, true.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wait Gameplay Event")
	bool bRequireSameSender = true;

	/** Timeout in seconds for security. (0 = no timeout, will wait forever (use it CAREFULLY) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wait Gameplay Event", meta = (ClampMin = "0.0"))
	float TimeoutSeconds = 10.0f;

	virtual EGameplayActionExecutionType GetExecutionType_Implementation() override { return EGameplayActionExecutionType::Latent; }
	virtual void ExecuteLatent_Implementation(UGameplayActionRuntime* ActionRuntime, const FEventContext& Context) override;

#if WITH_EDITOR
	virtual FString GetEditorSummary() const override
	{
		return FString::Printf(TEXT("Wait for %s"), *EventTagToWaitFor.ToString());
	}
#endif
};
