#pragma once

#include "CoreMinimal.h"
#include "Core/GameplayEventContext.h"
#include "GameplayCondition.generated.h"

/**
 * Base class for each condition evaluated when a GameplayEventDefinition
 * receives the corresponding event. Extend by legacy.
 *
 * NB : this class is NOT responsible for "which event triggers the evaluation".
 * This is handled by the GameplayEventDefinition::EventTag.
 * A condition only filtrates/Validates once the corresponding event already arrived.
 */

UCLASS(Abstract, EditInlineNew, BlueprintType, Blueprintable, DefaultToInstanced, CollapseCategories)
class GAMEPLAYORCHESTRATOR_API UGameplayCondition : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay Orchestrator|Condition")
	bool Evaluate(const FEventContext& Context);
	virtual bool Evaluate_Implementation(const FEventContext& Context) { return true; }

#if WITH_EDITOR
	virtual FString GetEditorSummary() const { return GetClass()->GetName(); }
#endif
};

/** AND group. Every condition must be validated */
USTRUCT(BlueprintType)
struct GAMEPLAYORCHESTRATOR_API FGameplayConditionAndGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Condition Group")
	TArray<TObjectPtr<UGameplayCondition>> Conditions;

	/** Empty group always means validation. */
	bool Evaluate(const FEventContext& Context) const
	{
		for (const TObjectPtr<UGameplayCondition>& Condition : Conditions)
		{
			if (!Condition || !Condition->Evaluate(Context))
			{
				return false;
			}
		}
		return true;
	}
};
