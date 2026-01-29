#include "UI/QuestObjectiveEntryWidget.h"
#include "QuestObjective.h"
#include "RepairObjective.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UQuestObjectiveEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

/*
 * Initializes the widget with the given quest objective.
 */
void UQuestObjectiveEntryWidget::InitializeObjective(UQuestObjective* InObjective)
{
	Objective = InObjective;

	if (!IsValid(Objective))
	{
		UE_LOG(LogTemp, Error, TEXT("UQuestObjectiveEntryWidget::InitializeObjective - Invalid Objective passed"));
		return;
	}

	if (ObjectiveDescriptionText)
	{
		ObjectiveDescriptionText->SetText(Objective->GetObjectiveDescription());
	}

	UpdateProgress();
}

/*
 * Updates the progress display and timer for the objective.
 */
void UQuestObjectiveEntryWidget::UpdateProgress()
{
	if (!IsValid(Objective))
	{
		UE_LOG(LogTemp, Error, TEXT("UQuestObjectiveEntryWidget::UpdateProgress - Invalid Objective"));
		return;
	}

	URepairObjective* RepairObj = Cast<URepairObjective>(Objective);
	if (IsValid(RepairObj))
	{
		int32 Completed = RepairObj->GetCompletedRepairPoints();
		int32 Total = RepairObj->GetTotalRepairPoints();
		float Progress = RepairObj->GetRepairProgress();

		if (ProgressText)
		{
			FText ProgressTextValue = FText::Format(
				FText::FromString("{0}/{1}"),
				FText::AsNumber(Completed),
				FText::AsNumber(Total)
			);
			ProgressText->SetText(ProgressTextValue);
		}

		float RemainingTime = RepairObj->GetRemainingTime();
		
		if (RepairObj->HasTimeLimit())
		{
			UpdateTimerDisplay(RemainingTime);
		}
		else if (TimerText)
		{
			TimerText->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (ProgressBar)
		{
			ProgressBar->SetPercent(Progress);
			
			FLinearColor BarColor = FLinearColor::LerpUsingHSV(
				FLinearColor::Red,
				FLinearColor::Green,
				Progress
			);
			ProgressBar->SetFillColorAndOpacity(BarColor);
		}
	}
}

/*
 * Updates the timer display based on remaining time.
 */
void UQuestObjectiveEntryWidget::UpdateTimerDisplay(float RemainingTime)
{
	if (!TimerText)
	{
		return;
	}

	TimerText->SetVisibility(ESlateVisibility::Visible);

	int32 Minutes = FMath::FloorToInt(RemainingTime / 60.0f);
	int32 Seconds = FMath::FloorToInt(RemainingTime) % 60;
	
	FString TimeString = FString::Printf(TEXT("⏱ %02d:%02d"), Minutes, Seconds);
	TimerText->SetText(FText::FromString(TimeString));

	if (TimerProgressBar)
	{
		URepairObjective* RepairObj = Cast<URepairObjective>(Objective);
		if (RepairObj && RepairObj->HasTimeLimit())
		{
			float TimePercent = RemainingTime / 300.0f;
			TimerProgressBar->SetPercent(TimePercent);
			TimerProgressBar->SetVisibility(ESlateVisibility::Visible);

			if (RemainingTime <= 30.0f)
			{
				TimerProgressBar->SetFillColorAndOpacity(FLinearColor::Red);
			}
			else if (RemainingTime <= 60.0f)
			{
				TimerProgressBar->SetFillColorAndOpacity(FLinearColor::Yellow);
			}
			else
			{
				TimerProgressBar->SetFillColorAndOpacity(FLinearColor(0.2f, 0.6f, 1.0f));
			}
		}
	}

	if (RemainingTime <= 30.0f)
	{
		TimerText->SetColorAndOpacity(FLinearColor::Red);
		
		float PulseScale = 1.0f + (FMath::Sin(RemainingTime * 10.0f) * 0.1f);
		TimerText->SetRenderScale(FVector2D(PulseScale, PulseScale));
	}
	else if (RemainingTime <= 60.0f)
	{
		TimerText->SetColorAndOpacity(FLinearColor::Yellow);
		TimerText->SetRenderScale(FVector2D(1.0f, 1.0f));
	}
	else
	{
		TimerText->SetColorAndOpacity(FLinearColor::White);
		TimerText->SetRenderScale(FVector2D(1.0f, 1.0f));
	}
}

/*
 * Plays the completion animation for the objective.
 */
void UQuestObjectiveEntryWidget::PlayCompletionAnimation()
{
	if (CompletionAnim)
	{
		PlayAnimation(CompletionAnim);
	}
}