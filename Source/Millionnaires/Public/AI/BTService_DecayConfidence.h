// BTService_DecayConfidence.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DecayConfidence.generated.h"

/**
 * Linearly decays the Blackboard's Confidence float key toward 0 over DecayDurationSeconds.
 * Attach this ONLY to the Investigate branch — not the tree root — so confidence doesn't drain
 * while the enemy is actively seeing the player (TargetVisible branch).
 */
UCLASS()
class MILLIONNAIRES_API UBTService_DecayConfidence : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_DecayConfidence();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ConfidenceKey;

	/** Seconds for Confidence to decay fully from 1.0 to 0.0. */
	UPROPERTY(EditAnywhere, Category = "Confidence", meta = (ClampMin = "0.1"))
	float DecayDurationSeconds = 20.f;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};