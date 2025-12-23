#include "UI/InteractionWidget.h"
#include "Components/TextBlock.h"

void UInteractionWidget::UpdateInteractionText(const FText& NewText)
{
	if (InteractionText)
	{
		InteractionText->SetText(NewText);
	}
}

void UInteractionWidget::SetInteractionVisible(bool bVisible)
{
	SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}