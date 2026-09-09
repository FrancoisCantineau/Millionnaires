// BTTask_ChooseNeutralAction.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChooseNeutralAction.generated.h"

UENUM(BlueprintType)
enum class ENeutralAction : uint8
{
	Wander,
	Idle,
	Observe,
	MAX UMETA(Hidden) // Sentinel only, never an actual action — always equals the count of real values above it, so the random range in the .cpp never needs manual updating.
};

/**
 * Picks a random neutral behavior (Wander, Idle, or Observe) and writes it to the Blackboard.
 * Each branch below this task in the tree is gated by a UBTDecorator_NeutralActionIs checking
 * NeutralActionKey against a specific value. Instant task — completes the same tick it runs.
 */
UCLASS()
class MILLIONNAIRES_API UBTTask_ChooseNeutralAction : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ChooseNeutralAction();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NeutralActionKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif
};