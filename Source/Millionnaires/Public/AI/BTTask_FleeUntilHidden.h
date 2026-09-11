// BTTask_FleeUntilHidden.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FleeUntilHidden.generated.h"

/**
 * Latent task: moves toward EscapeLocation, periodically checking whether TargetActor can still
 * see the controlled Pawn. The instant it can't, stops moving and succeeds — it never decides
 * what happens next (disappear for good, reappear near the player, stay put quietly, etc.);
 * that's a separate decision one level up in the BT. This task only ever means "got hidden". The
 * internal visibility polling itself is not a strategic decision, so it lives here rather than as
 * a Decorator/Service pair on a generic Move To.
 */
UCLASS()
class MILLIONNAIRES_API UBTTask_FleeUntilHidden : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FleeUntilHidden();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector EscapeLocationKey;

	UPROPERTY(EditAnywhere, Category = "Flee", meta = (ClampMin = "0.05"))
	float VisibilityCheckInterval = 0.2f;

	/** If the target still hasn't lost sight of the enemy after this long, give up and fail
	 *  rather than fleeing forever — lets the rest of the tree fall back to something else. */
	UPROPERTY(EditAnywhere, Category = "Flee", meta = (ClampMin = "1.0"))
	float MaxDurationSeconds = 8.f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

private:
	struct FFleeMemory
	{
		float TimeSinceLastCheck = 0.f;
		float TotalElapsed = 0.f;
	};
};