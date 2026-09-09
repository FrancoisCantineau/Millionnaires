// BTTask_FirstReaction.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FirstReaction.generated.h"

/**
 * Latent task for the very first detection out of Neutral: tells LookAtComponent to face
 * TargetActor, waits for the blend to actually reach full weight (so it waits for the real
 * visual turn, wherever that logic lives in the AnimGraph — not a fixed guess), then holds for
 * PauseDurationSeconds before clearing the look-at and succeeding (letting Chase take over).
 *
 * No rotation, no animation, no bone logic here — purely decides WHEN to look and for how long.
 * Requires a ULookAtComponent on the controlled Pawn.
 */
UCLASS()
class MILLIONNAIRES_API UBTTask_FirstReaction : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FirstReaction();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	/** Cleared to false the instant this task finishes — this is the ONLY place ReactionPending
	 *  ever gets consumed. */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ReactionPendingKey;

	/** How close LookAtComponent's blend weight must get to 1.0 before considering the character
	 *  "facing" the target — the actual turn speed lives in the AnimGraph/LookAtComponent, not here. */
	UPROPERTY(EditAnywhere, Category = "FirstReaction", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LookAtAlphaThreshold = 0.95f;

	UPROPERTY(EditAnywhere, Category = "FirstReaction")
	float PauseDurationSeconds = 1.5f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

private:
	enum class EFirstReactionStep : uint8
	{
		WaitingToFaceTarget,
		Pausing,
		Finished
	};

	struct FFirstReactionMemory
	{
		float PauseElapsed = 0.f;
		EFirstReactionStep Step = EFirstReactionStep::WaitingToFaceTarget;
	};
};