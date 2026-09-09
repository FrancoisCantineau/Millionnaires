// EnemyAIController.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UBehaviorTree;
struct FAIStimulus;

/**
 * V0 of the main enemy's controller. Deliberately minimal: perceives the player (sight/hearing),
 * writes ONLY the 3 pieces of information the enemy is allowed to know (see design notes) into
 * the Blackboard, and starts the Behavior Tree. No decision logic lives here — that's the BT's job.
 */
UCLASS()
class MILLIONNAIRES_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

public:
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	/** Confidence assigned the instant sight is lost — perception was perfect right up to that moment. */
	UPROPERTY(EditAnywhere, Category = "AI|Confidence", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ConfidenceOnSightLoss = 1.0f;

	/** Confidence assigned when only a noise was heard — deliberately much lower than sight; a
	 *  noise means "something happened over there", never "the player is definitely there". */
	UPROPERTY(EditAnywhere, Category = "AI|Confidence", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ConfidenceOnNoise = 0.3f;

	/** Blackboard key names — exposed so you can rename them to match your Blackboard asset without recompiling. */
	UPROPERTY(EditAnywhere, Category = "AI|Blackboard Keys")
	FName TargetActorKeyName = TEXT("TargetActor");

	UPROPERTY(EditAnywhere, Category = "AI|Blackboard Keys")
	FName LastKnownLocationKeyName = TEXT("LastKnownLocation");

	UPROPERTY(EditAnywhere, Category = "AI|Blackboard Keys")
	FName TargetVisibleKeyName = TEXT("TargetVisible");

	UPROPERTY(EditAnywhere, Category = "AI|Blackboard Keys")
	FName ConfidenceKeyName = TEXT("Confidence");

protected:
	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;
};