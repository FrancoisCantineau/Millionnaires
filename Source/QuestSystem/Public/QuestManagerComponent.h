#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestSystemInterface.h"

#include "QuestManagerComponent.generated.h"

class UQuestObjective;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveStarted, UQuestObjective*, Objective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveCompleted, UQuestObjective*, Objective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveFailed, UQuestObjective*, Objective);

/**
 * Component managing quest objectives lifecycle
 * - Starting, updating, completing, failing objectives
 * - Broadcasting events for objective state changes
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class QUESTSYSTEM_API UQuestManagerComponent : public UActorComponent, public IQuestSystemInterface
{
	GENERATED_BODY()

public:	
	
	UQuestManagerComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Start a new objective */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void StartObjective(UQuestObjective* Objective);

	/** Get current active objective */
	UFUNCTION(BlueprintPure, Category = "Quest")
	UQuestObjective* GetCurrentObjective() const { return CurrentObjective; }

	/** Called when an objective is completed */
	void OnObjectiveCompleted(UQuestObjective* Objective);

	/** Called when an objective fails */
	void OnObjectiveFailed(UQuestObjective* Objective);

	/** Event dispatched when an objective starts */
	UPROPERTY(BlueprintAssignable, Category = "Quest Events")
	FOnObjectiveStarted OnObjectiveStartedEvent;

	/** Event dispatched when an objective is completed */
	UPROPERTY(BlueprintAssignable, Category = "Quest Events")
	FOnObjectiveCompleted OnObjectiveCompletedEvent;

	/** Event dispatched when an objective fails */
	UPROPERTY(BlueprintAssignable, Category = "Quest Events")
	FOnObjectiveFailed OnObjectiveFailedEvent;

	/** IQuestSystemInterface implementation */
	virtual void StartDebugObjective(const FString& ObjectiveClassName) override;

protected:

	virtual void BeginPlay() override;

	/** Return to dispatch mode */
	void ReturnToDispatch();

	/** Clear current objective and disable tick */
	void ClearCurrentObjective();

	/** Find objective class by name */
	UClass* FindObjectiveClass(const FString& ClassName);

	/** Current active objective */
	UPROPERTY()
	UQuestObjective* CurrentObjective;

	/** Debug: Auto-start objective on begin play */
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bAutoStartDebugObjective;

	/** Debug: Objective class to auto-start */
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (EditCondition = "bAutoStartDebugObjective"))
	TSubclassOf<UQuestObjective> DebugStartObjective;
};