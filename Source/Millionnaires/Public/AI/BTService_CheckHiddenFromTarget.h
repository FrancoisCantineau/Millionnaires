// BTService_CheckHiddenFromTarget.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckHiddenFromTarget.generated.h"

/**
 * Continuously checks whether TargetActor can currently see the controlled Pawn, writing the
 * result to a Bool Blackboard key. Attach to the Flee branch (or specifically the Sequence
 * containing Move To EscapeLocation) alongside a native "Blackboard" Decorator checking
 * TargetCanSeeEnemy == false (Observer Aborts: Self) — this lets Flee interrupt its movement the
 * instant line of sight breaks, without waiting for Move To to finish.
 */
UCLASS()
class MILLIONNAIRES_API UBTService_CheckHiddenFromTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckHiddenFromTarget();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	/** Set to true once TargetActor can no longer see the enemy. */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetCanSeeEnemyKey;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};