// DialogueWidgetBase.cpp
#include "DialogueWidgetBase.h"
#include "DialogueSpeaker.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"

void UDialogueWidgetBase::OnLineStarted_Implementation(const FDialogueLine& Line)
{
	if (LineText)
	{
		LineText->SetText(Line.LineText);
	}

	if (SpeakerNameText)
	{
		SpeakerNameText->SetText(Line.Speaker ? Line.Speaker->DisplayName : FText::GetEmpty());
		if (Line.Speaker)
		{
			SpeakerNameText->SetColorAndOpacity(FSlateColor(Line.Speaker->NameColor));
		}
	}

	if (SpeakerPortraitImage)
	{
		if (Line.Speaker && Line.Speaker->Portrait)
		{
			SpeakerPortraitImage->SetBrushFromTexture(Line.Speaker->Portrait);
			SpeakerPortraitImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			SpeakerPortraitImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Clear any leftover choice buttons from a previous branching node.
	if (ChoicesContainer)
	{
		ChoicesContainer->ClearChildren();
	}
}

void UDialogueWidgetBase::OnChoicesPresented_Implementation(const TArray<FDialogueChoice>& Choices)
{
	if (!ChoicesContainer || !ChoiceButtonClass)
	{
		return;
	}

	ChoicesContainer->ClearChildren();

	for (int32 i = 0; i < Choices.Num(); ++i)
	{
		UDialogueChoiceButtonBase* ChoiceWidget = CreateWidget<UDialogueChoiceButtonBase>(this, ChoiceButtonClass);
		if (ChoiceWidget)
		{
			ChoiceWidget->SetupChoice(Choices[i], i, OwningDialogueComponent);
			ChoicesContainer->AddChild(ChoiceWidget);
		}
	}
}

void UDialogueWidgetBase::OnDialogueEnded_Implementation()
{
	if (ChoicesContainer)
	{
		ChoicesContainer->ClearChildren();
	}
}
