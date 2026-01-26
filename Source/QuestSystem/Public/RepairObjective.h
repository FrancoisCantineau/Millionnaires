#pragma once

#include "CoreMinimal.h"
#include "QuestObjective.h"

#include "RepairObjective.generated.h"

class ARepairableEquipmentActor;

/** 
 * Quest objective for repairing multiple pieces of equipment within an optional time limit
 * 
 * - Tracks multiple RepairableEquipmentActors
 * - Can auto-discover repair points in the world
 * - Provides progress tracking and time limit functionality
 */
UCLASS()
class QUESTSYSTEM_API URepairObjective : public UQuestObjective
{
	GENERATED_BODY()

public:
	
	URepairObjective();

	virtual void ObjectiveActivate() override;
	virtual void TickObjective(float DeltaTime) override;

	/** Add a repair point to track */
	void AddRepairPoint(ARepairableEquipmentActor* RepairPoint);

	/** Called when a repair point is completed */
	void OnRepairPointCompleted(ARepairableEquipmentActor* RepairPoint);

	/** Get repair progress */
	UFUNCTION(BlueprintPure, Category = "Quest")
	float GetRepairProgress() const;

	/** Get completed repair points count */
	UFUNCTION(BlueprintPure, Category = "Quest")
	int32 GetCompletedRepairPoints() const { return CompletedCount; }

	/** Get total repair points count */
	UFUNCTION(BlueprintPure, Category = "Quest")
	int32 GetTotalRepairPoints() const { return RepairPoints.Num(); }

	/** Get remaining time */
	UFUNCTION(BlueprintPure, Category = "Quest")
	float GetRemainingTime() const { return RemainingTime; }

	/** Check if timer is enabled */
	UFUNCTION(BlueprintPure, Category = "Quest")
	bool HasTimeLimit() const { return bHasTimeLimit; }

protected:
	
	/** Auto-discover all RepairableEquipmentActors in the world */
	void AutoDiscoverRepairPoints();

	/** Called when time runs out */
	void OnTimeLimitReached();

	/** All repair points in this objective */
	UPROPERTY()
	TArray<ARepairableEquipmentActor*> RepairPoints;

	/** Number of completed repairs */
	UPROPERTY()
	int32 CompletedCount;

	/** Enable time limit */
	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	bool bHasTimeLimit = false;

	/** Time limit in seconds */
	UPROPERTY(EditDefaultsOnly, Category = "Timer", meta = (EditCondition = "bHasTimeLimit"))
	float TimeLimit = 300.0f;

	/** Remaining time */
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	float RemainingTime;

	/** Warning threshold */
	UPROPERTY(EditDefaultsOnly, Category = "Timer", meta = (EditCondition = "bHasTimeLimit"))
	float WarningTimeThreshold = 60.0f;

	/** Has warning been shown */
	UPROPERTY()
	bool bWarningShown = false;
};