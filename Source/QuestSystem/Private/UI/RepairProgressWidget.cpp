#include "UI/RepairProgressWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void URepairProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentProgress = 0.0f;

	if (ProgressBar)
	{
		ProgressBar->SetPercent(0.0f);
	}

	if (ProgressText)
	{
		ProgressText->SetText(FText::FromString("0%"));
	}
}

/*
 * Sets the repair progress value
 * 
 * @param Progress - Value between 0.0 and 1.0
 */
void URepairProgressWidget::SetProgress(float Progress)
{
	CurrentProgress = FMath::Clamp(Progress, 0.0f, 1.0f);

	if (ProgressBar)
	{
		ProgressBar->SetPercent(CurrentProgress);
	}

	if (ProgressText)
	{
		int32 Percentage = FMath::RoundToInt(CurrentProgress * 100.0f);
		ProgressText->SetText(FText::Format(
			FText::FromString("{0}%"),
			FText::AsNumber(Percentage)
		));
	}
}

/*
 * Sets the equipment name text
 * 
 * @param Name - Equipment name to display
 */
void URepairProgressWidget::SetEquipmentName(const FText& Name)
{
	if (EquipmentNameText)
	{
		EquipmentNameText->SetText(Name);
	}
}