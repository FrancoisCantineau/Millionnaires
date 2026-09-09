// BTService_FaceTarget.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FaceTarget.generated.h"

/**
 * Continuously calls SetFocalPoint toward TargetActor while attached to an active branch (e.g.
 * Chase), so bUseControllerDesiredRotation smoothly tracks the target the whole time — not just
 * during FirstReaction. Clears the focal point via OnCeaseRelevant the instant the branch stops
 * being active, so the character doesn't keep facing a stale target after Chase ends.
 */
UCLASS()
class MILLIONNAIRES_API UBTService_FaceTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FaceTarget();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};