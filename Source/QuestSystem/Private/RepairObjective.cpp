#include "RepairObjective.h"

#include "RepairableEquipmentActor.h"
#include "EngineUtils.h"
#include "QuestManagerComponent.h"

#pragma region Initialization

/*
 * Constructor
 * - Initializes default values
 */
URepairObjective::URepairObjective()
{
	CompletedCount = 0;
	ObjectiveDescription = FText::FromString("Repair all equipment");
	RemainingTime = 0.0f;
	bWarningShown = false;
}

/*
 * Activate the repair objective
 * - Enables all repair points
 * - Starts timer if applicable
 */
void URepairObjective::ObjectiveActivate()
{
	Super::ObjectiveActivate();

	if (RepairPoints.Num() == 0)
	{
		AutoDiscoverRepairPoints();
	}

	for (ARepairableEquipmentActor* RepairPoint : RepairPoints)
	{
		if (RepairPoint)
		{
			RepairPoint->EnableRepair();
		}
	}

	if (bHasTimeLimit)
	{
		RemainingTime = TimeLimit;
	}
}

/*
 * Tick the repair objective
 * - Updates timer if applicable
 * - Checks for time warnings and expiration
 * 
 * @param DeltaTime - Time since last tick
 */
void URepairObjective::TickObjective(float DeltaTime)
{
	Super::TickObjective(DeltaTime);

	if (bHasTimeLimit && RemainingTime > 0.0f)
	{
		RemainingTime -= DeltaTime;

		if (!bWarningShown && RemainingTime <= WarningTimeThreshold)
		{
			bWarningShown = true;
		}

		if (RemainingTime <= 0.0f)
		{
			RemainingTime = 0.0f;
			OnTimeLimitReached();
		}
	}
}

#pragma endregion

#pragma region TimeLimit

void URepairObjective::OnTimeLimitReached()
{
	ObjectiveFail();
}

#pragma endregion

#pragma region RepairPointsManagement

/** 
 * Add a repair point to the objective list and set ownership
 * 
 * @param RepairPoint - The repairable equipment actor to add
 */
void URepairObjective::AddRepairPoint(ARepairableEquipmentActor* RepairPoint)
{
	if (!RepairPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("URepairObjective::AddRepairPoint - RepairPoint is null"));
		return;
	}

	if (RepairPoints.Contains(RepairPoint))
	{
		UE_LOG(LogTemp, Warning, TEXT("URepairObjective::AddRepairPoint - RepairPoint already in list"));
		return;
	}

	RepairPoints.Add(RepairPoint);
	RepairPoint->SetOwningObjective(this);
}

/** 
 * Called when a repair point is completed and updates progress
 * 
 * @param RepairPoint - The repairable equipment actor that was completed
 */
void URepairObjective::OnRepairPointCompleted(ARepairableEquipmentActor* RepairPoint)
{
	if (!RepairPoints.Contains(RepairPoint))
	{
		UE_LOG(LogTemp, Warning, TEXT("URepairObjective::OnRepairPointCompleted - RepairPoint not part of this objective"));
		return;
	}

	CompletedCount++;
	if (CompletedCount >= RepairPoints.Num())
	{
		ObjectiveComplete();
	}
}

#pragma endregion

/** 
 * Get the current repair progress as a float between 0.0 and 1.0
 * 
 * @return Repair progress ratio
 */
float URepairObjective::GetRepairProgress() const
{
	if (RepairPoints.Num() == 0)
	{
		return 0.0f;
	}

	return static_cast<float>(CompletedCount) / static_cast<float>(RepairPoints.Num());
}

/** 
 * Auto-discover all RepairableEquipmentActors in the world and add them as repair points
 */
void URepairObjective::AutoDiscoverRepairPoints()
{
	if (!QuestManager)
	{
		UE_LOG(LogTemp, Error, TEXT("URepairObjective::AutoDiscoverRepairPoints - No QuestManager assigned"));
		return;
	}

	UWorld* World = QuestManager->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("URepairObjective::AutoDiscoverRepairPoints - Unable to get World from QuestManager"));
		return;
	}

	for (TActorIterator<ARepairableEquipmentActor> It(World); It; ++It)
	{
		ARepairableEquipmentActor* RepairActor = *It;
		if (RepairActor)
		{
			AddRepairPoint(RepairActor);
		}
	}
}