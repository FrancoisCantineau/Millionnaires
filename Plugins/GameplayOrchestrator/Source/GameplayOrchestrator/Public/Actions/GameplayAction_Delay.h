#pragma once

#include "CoreMinimal.h"
#include "GameplayActionBase.h"
#include "GameplayAction_Delay.generated.h"

/**
 * Wait N secondes before calling the end.
 */
UCLASS(meta = (DisplayName = "Delay"))
class GAMEPLAYORCHESTRATOR_API UGameplayAction_Delay : public UGameplayActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delay", meta = (ClampMin = "0.0"))
	float Duration = 1.0f;

	virtual EGameplayActionExecutionType GetExecutionType_Implementation() override { return EGameplayActionExecutionType::Latent; }
	virtual void ExecuteLatent_Implementation(UGameplayActionRuntime* ActionRuntime, const FEventContext& Context) override;

#if WITH_EDITOR
	virtual FString GetEditorSummary() const override
	{
		return FString::Printf(TEXT("Delay %.2fs"), Duration);
	}
#endif
};

