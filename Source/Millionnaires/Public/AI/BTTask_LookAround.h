// BTTask_LookAround.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_LookAround.generated.h"

/**
 * Latent task: looks left, pauses, swings right (past center), pauses, then returns to neutral.
 * Drives LookAtComponent with computed world points (not an actor) — no rotation, no animation
 * logic here, purely decides where and how long to look. Requires a ULookAtComponent on the
 * controlled Pawn.
 */
UCLASS()
class MILLIONNAIRES_API UBTTask_LookAround : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_LookAround();

	/** How far to turn left from the starting facing. */
	UPROPERTY(EditAnywhere, Category = "LookAround")
	float LeftAmplitudeDegrees = 90.f;

	/** How far to swing right FROM THE LEFT EXTREME (180° covers left-extreme to right-extreme). */
	UPROPERTY(EditAnywhere, Category = "LookAround")
	float RightSwingDegreesFromLeftExtreme = 180.f;

	UPROPERTY(EditAnywhere, Category = "LookAround")
	float PauseDurationSeconds = 0.5f;

	/** How far in front of the pawn to project the look points — LookAtComponent needs an actual
	 *  world location, not just a direction. */
	UPROPERTY(EditAnywhere, Category = "LookAround", meta = (ClampMin = "10.0"))
	float LookDistance = 500.f;

	/** How close the blend weight must get to the target (1.0 when looking, 0.0 when returned to
	 *  neutral) before considering that step complete. */
	UPROPERTY(EditAnywhere, Category = "LookAround", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AlphaThreshold = 0.95f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

private:
	enum class ELookAroundStep : uint8
	{
		LookingLeft,
		PausingAtLeft,
		LookingRight,
		PausingAtRight,
		ReturningToCenter,
		Finished
	};

	struct FLookAroundMemory
	{
		FVector LeftPoint = FVector::ZeroVector;
		FVector RightPoint = FVector::ZeroVector;
		float PauseElapsed = 0.f;
		ELookAroundStep Step = ELookAroundStep::LookingLeft;
	};
};