#pragma once

#include "CoreMinimal.h"
#include "Conditions/GameplayCondition.h"
#include "GameplayCondition_StateEquals.generated.h"

/** Checks a state in the WorldStateSubsystem.
 * For ex : Reacting to a doors opening event, only when a precise quest is activated.
 * Condition : - StateEquals : "Mission.Phase" == "State.ReturnPhase"
 */
UCLASS(meta = (DisplayName = "State Equals"))
class GAMEPLAYORCHESTRATOR_API UGameplayCondition_StateEquals : public UGameplayCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Equals")
	FGameplayTag StateKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Equals")
	FGameplayTag ExpectedValue;

	virtual bool Evaluate_Implementation(const FEventContext& Context) override;

#if WITH_EDITOR
	virtual FString GetEditorSummary() const override;
#endif
};
