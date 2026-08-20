#include "UI/QuestJournalObjectiveEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UQuestJournalObjectiveEntryWidget::SetEntry(const FQuestJournalObjectiveEntry& InEntry)
{
	Entry = InEntry;

	if (DescriptionText)
	{
		DescriptionText->SetText(Entry.Description);
	}

	if (Entry.bHasProgress)
	{
		if (ProgressText)
		{
			ProgressText->SetVisibility(ESlateVisibility::Visible);
			ProgressText->SetText(FText::Format(FText::FromString(TEXT("{0}/{1}")), Entry.ProgressCurrent, Entry.ProgressTarget));
		}
		if (ProgressBar)
		{
			ProgressBar->SetVisibility(ESlateVisibility::Visible);
			ProgressBar->SetPercent(Entry.ProgressTarget > 0 ? (float)Entry.ProgressCurrent / (float)Entry.ProgressTarget : 0.0f);
		}
	}
	else
	{
		if (ProgressText)
		{
			ProgressText->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (ProgressBar)
		{
			ProgressBar->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (TimerText)
	{
		TimerText->SetVisibility(Entry.bHasTimeLimit ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	OnEntrySet();
}

void UQuestJournalObjectiveEntryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!Entry.bHasTimeLimit || !TimerText || !GetWorld())
	{
		return;
	}

	// Purely local computation, no network call: ActivationServerTime and TimeLimitSeconds are
	// one-shot values already sitting in Entry, refreshed only when the quest state actually
	// changes (not every tick) - see UQuestJournalWidget::RefreshEntries.
	float Elapsed = GetWorld()->GetTimeSeconds() - Entry.ActivationServerTime;
	float Remaining = FMath::Max(0.0f, Entry.TimeLimitSeconds - Elapsed);

	int32 Minutes = FMath::FloorToInt(Remaining / 60.0f);
	int32 Seconds = FMath::FloorToInt(Remaining) % 60;
	TimerText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
}
