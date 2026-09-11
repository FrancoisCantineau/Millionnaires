// BTTask_SetMovementSpeed.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BTTask_SetMovementSpeed.generated.h"

/**
 * Instant task: applies a named speed profile via SpeedProfileComponent, looked up by tag. This
 * task knows nothing about what kind of Pawn it's applied to — the tag->speed mapping lives on
 * that Pawn's own SpeedProfileComponent instance.
 */
UCLASS()
class MOVEMENTSPEEDSYSTEM_API UBTTask_SetMovementSpeed : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetMovementSpeed();

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (Categories = "Movement.Speed"))
	FGameplayTag SpeedProfile;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif
};
