// DialogueChoiceButtonBase.cpp
#include "DialogueChoiceButtonBase.h"
#include "DialogueComponent.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UDialogueChoiceButtonBase::SetupChoice_Implementation(const FDialogueChoice& Choice, int32 ChoiceIndex, UDialogueComponent* OwningComponent)
{
	if (ChoiceLabel)
	{
		ChoiceLabel->SetText(Choice.ChoiceText);
	}

	CachedComponent = OwningComponent;
	CachedIndex = ChoiceIndex;

	if (ChoiceButton)
	{
		// RemoveDynamic first in case this widget instance gets reused/re-setup.
		ChoiceButton->OnClicked.RemoveDynamic(this, &UDialogueChoiceButtonBase::HandleClicked);
		ChoiceButton->OnClicked.AddDynamic(this, &UDialogueChoiceButtonBase::HandleClicked);
	}
}

void UDialogueChoiceButtonBase::HandleClicked()
{
	if (CachedComponent)
	{
		CachedComponent->SelectChoice(CachedIndex);
	}
}
