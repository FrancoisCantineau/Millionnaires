// ContainerWidgetBase.cpp
#include "ContainerWidgetBase.h"
#include "InventoryComponent.h"
#include "Components/PanelWidget.h"

void UContainerWidgetBase::SetContainer(UInventoryComponent* NewContainer)
{
	if (Container == NewContainer)
	{
		return;
	}

	if (Container)
	{
		Container->OnInventoryChangedDelegate.RemoveDynamic(this, &UContainerWidgetBase::HandleInventoryChanged);
	}

	Container = NewContainer;

	if (Container)
	{
		Container->OnInventoryChangedDelegate.AddDynamic(this, &UContainerWidgetBase::HandleInventoryChanged);
	}

	RefreshSlots();
}

void UContainerWidgetBase::HandleInventoryChanged()
{
	RefreshSlots();
}

void UContainerWidgetBase::NativeDestruct()
{
	if (Container)
	{
		Container->OnInventoryChangedDelegate.RemoveDynamic(this, &UContainerWidgetBase::HandleInventoryChanged);
	}
	Super::NativeDestruct();
}

void UContainerWidgetBase::RefreshSlots()
{
	if (!SlotsContainer || !SlotWidgetClass || !Container)
	{
		return;
	}

	SlotsContainer->ClearChildren();
	SlotWidgets.Reset();
	DisplayedSlotIndices.Reset();

	const TArray<FInventorySlot>& Slots = Container->GetSlots();
	for (int32 RealIndex = 0; RealIndex < Slots.Num(); ++RealIndex)
	{
		// Skip empty slots entirely — this is a compact list (like a dropdown), not a fixed grid,
		// so an emptied slot doesn't leave a visible gap; everything below shifts up.
		if (Slots[RealIndex].IsEmpty())
		{
			continue;
		}

		UContainerSlotWidgetBase* SlotWidget = CreateWidget<UContainerSlotWidgetBase>(this, SlotWidgetClass);
		if (SlotWidget)
		{
			SlotWidget->SetupSlot(Slots[RealIndex], RealIndex);
			SlotsContainer->AddChild(SlotWidget);
			SlotWidgets.Add(SlotWidget);
			DisplayedSlotIndices.Add(RealIndex);
		}
	}

	SelectedIndex = FMath::Clamp(SelectedIndex, 0, FMath::Max(SlotWidgets.Num() - 1, 0));
	UpdateHighlight();
}

void UContainerWidgetBase::MoveSelection(int32 Delta)
{
	if (SlotWidgets.Num() == 0)
	{
		return;
	}

	SelectedIndex = (SelectedIndex + Delta + SlotWidgets.Num()) % SlotWidgets.Num();
	UpdateHighlight();
}

int32 UContainerWidgetBase::GetSelectedSlotIndex() const
{
	return DisplayedSlotIndices.IsValidIndex(SelectedIndex) ? DisplayedSlotIndices[SelectedIndex] : INDEX_NONE;
}

void UContainerWidgetBase::UpdateHighlight()
{
	for (int32 i = 0; i < SlotWidgets.Num(); ++i)
	{
		if (SlotWidgets[i])
		{
			SlotWidgets[i]->SetHighlighted(i == SelectedIndex);
		}
	}
}
