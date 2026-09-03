// ContainerSlotWidgetBase.cpp
#include "ContainerSlotWidgetBase.h"
#include "ItemDefinition.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UContainerSlotWidgetBase::SetupSlot_Implementation(const FInventorySlot& InSlot, int32 InSlotIndex)
{
	CachedSlotIndex = InSlotIndex;

	if (ItemNameText)
	{
		ItemNameText->SetText(InSlot.ItemDef ? InSlot.ItemDef->DisplayName : FText::GetEmpty());
	}

	if (QuantityText)
	{
		if (InSlot.IsEmpty() || InSlot.Quantity <= 1)
		{
			QuantityText->SetText(FText::GetEmpty());
		}
		else
		{
			QuantityText->SetText(FText::AsNumber(InSlot.Quantity));
		}
	}

	if (IconImage)
	{
		if (InSlot.ItemDef && InSlot.ItemDef->Icon)
		{
			IconImage->SetBrushFromTexture(InSlot.ItemDef->Icon);
			IconImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			IconImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UContainerSlotWidgetBase::SetHighlighted_Implementation(bool bHighlighted)
{
	SetRenderOpacity(bHighlighted ? 1.0f : 0.6f);
}
