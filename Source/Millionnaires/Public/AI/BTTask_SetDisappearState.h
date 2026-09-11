#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Components/DisappearComponent.h"
#include "BTTask_SetDisappearState.generated.h"

/**
 * Instant task: calls SetDisappearState() on the Pawn's DisappearComponent. EDisappearState
 * itself lives in DisappearComponent.h, not here — it's a concept of the disappear mechanic's
 * domain, not something specific to this Task (a cutscene or trigger might set it too).
 */
UCLASS()
class MILLIONNAIRES_API UBTTask_SetDisappearState : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetDisappearState();

	UPROPERTY(EditAnywhere, Category = "Disappear")
	EDisappearState State = EDisappearState::Disappeared;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif
};