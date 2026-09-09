// BTDecorator_ConfAboveThreshold.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_ConfAboveThreshold.generated.h"

/**
 * Passes only if the Blackboard's Confidence float key is above Threshold. Used to gate the
 * "search" sub-branch of Investigate — below threshold, the branch fails and the Selector falls
 * through to Neutral instead.
 */
UCLASS()
class MILLIONNAIRES_API UBTDecorator_ConfAboveThreshold : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_ConfAboveThreshold();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ConfidenceKey;

	UPROPERTY(EditAnywhere, Category = "Condition", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Threshold = 0.5f;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif
};