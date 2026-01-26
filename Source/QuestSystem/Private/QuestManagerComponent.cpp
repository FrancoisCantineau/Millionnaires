#include "QuestManagerComponent.h"

#include "QuestObjective.h"

#pragma region Initialization

UQuestManagerComponent::UQuestManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	CurrentObjective = nullptr;
	bAutoStartDebugObjective = false;
}

/*
 * BeginPlay
 * - Automatically starts a debug objective if configured
 */
void UQuestManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoStartDebugObjective && DebugStartObjective)
	{
		UQuestObjective* NewObjective = NewObject<UQuestObjective>(this, DebugStartObjective);
		if (NewObjective)
		{
			StartObjective(NewObjective);
		}
	}
}

/*
 * TickComponent
 * - Updates the current active objective each frame
 */
void UQuestManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentObjective && CurrentObjective->IsActive())
	{
		CurrentObjective->TickObjective(DeltaTime);
	}
}

#pragma endregion

#pragma region Objective Management

/*
 * StartObjective
 * - Initializes and activates a new objective
 * - Disables if there's already an active objective
 * - Broadcasts OnObjectiveStartedEvent
 * 
 * @param Objective The objective to start
 */
void UQuestManagerComponent::StartObjective(UQuestObjective* Objective)
{
	if (!Objective || CurrentObjective)
	{
		UE_LOG(LogTemp, Error, TEXT("UQuestManagerComponent::StartObjective - Cannot start objective"));
		return;
	}

	CurrentObjective = Objective;
	CurrentObjective->Initialize(this);
	CurrentObjective->ObjectiveActivate();

	SetComponentTickEnabled(true);
	OnObjectiveStartedEvent.Broadcast(CurrentObjective);
}

/*
 * OnObjectiveCompleted
 * - Handles completion of the current objective
 * - Broadcasts OnObjectiveCompletedEvent
 * - Clears current objective and returns to dispatch mode
 * 
 * @param Objective The completed objective
 */
void UQuestManagerComponent::OnObjectiveCompleted(UQuestObjective* Objective)
{
	if (Objective != CurrentObjective)
	{
		UE_LOG(LogTemp, Error, TEXT("UQuestManagerComponent::OnObjectiveCompleted - Objective does not match current objective"));
		return;
	}

	OnObjectiveCompletedEvent.Broadcast(Objective);
	ClearCurrentObjective();
	ReturnToDispatch();
}

/*
 * OnObjectiveFailed
 * - Handles failure of the current objective
 * - Broadcasts OnObjectiveFailedEvent
 * - Clears current objective
 * 
 * @param Objective The failed objective
 */
void UQuestManagerComponent::OnObjectiveFailed(UQuestObjective* Objective)
{
	if (Objective != CurrentObjective)
	{
		UE_LOG(LogTemp, Error, TEXT("UQuestManagerComponent::OnObjectiveFailed - Objective does not match current objective"));
		return;
	}

	OnObjectiveFailedEvent.Broadcast(Objective);
	ClearCurrentObjective();
}

/*
 * ClearCurrentObjective
 * - Clears the current objective and disables ticking
 */
void UQuestManagerComponent::ClearCurrentObjective()
{
	CurrentObjective = nullptr;
	SetComponentTickEnabled(false);
}

/*
 * ReturnToDispatch
 * - Transitions the system back to dispatch mode
 */
void UQuestManagerComponent::ReturnToDispatch()
{
	// TODO: Implement transition to dispatch mode
}

#pragma endregion

#pragma region Debug

/*
 * StartDebugObjective
 * - Finds and starts an objective by class name for debugging purposes
 * 
 * @param ObjectiveClassName The name of the objective class to start
 */
void UQuestManagerComponent::StartDebugObjective(const FString& ObjectiveClassName)
{
	UClass* ObjectiveClass = FindObjectiveClass(ObjectiveClassName);
	
	if (!ObjectiveClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UQuestManagerComponent::StartDebugObjective - Objective class '%s' not found."), *ObjectiveClassName);
		return;
	}

	UQuestObjective* NewObjective = NewObject<UQuestObjective>(this, ObjectiveClass);
	if (NewObjective)
	{
		StartObjective(NewObjective);
	}
}

/*
 * FindObjectiveClass
 * - Searches for a UQuestObjective subclass by name
 * - Logs available objective classes if not found
 * 
 * @param ClassName The name of the objective class to find
 * @return The UClass pointer if found, nullptr otherwise
 */
UClass* UQuestManagerComponent::FindObjectiveClass(const FString& ClassName)
{
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
        
		if (!Class->IsChildOf(UQuestObjective::StaticClass()) || Class->HasAnyClassFlags(CLASS_Abstract))
		{
			continue;
		}
        
		FString ClassSimpleName = Class->GetName();
		if (ClassSimpleName.Equals(ClassName, ESearchCase::IgnoreCase))
		{
			return Class;
		}
	}
	return nullptr;
}

#pragma endregion