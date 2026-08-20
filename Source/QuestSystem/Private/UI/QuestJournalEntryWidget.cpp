#include "UI/QuestJournalEntryWidget.h"
#include "Components/TextBlock.h"

void UQuestJournalEntryWidget::SetEntry(const FQuestJournalEntry& InEntry)
{
	Entry = InEntry;

	if (TitleText)
	{
		TitleText->SetText(Entry.Title);
	}
	if (DescriptionText)
	{
		DescriptionText->SetText(Entry.Description);
	}
	if (QuestTypeText)
	{
		QuestTypeText->SetText(Entry.QuestType == EQuestType::Main ? FText::FromString(TEXT("Main Quest")) : FText::FromString(TEXT("Side Quest")));
	}

	OnEntrySet();
}
