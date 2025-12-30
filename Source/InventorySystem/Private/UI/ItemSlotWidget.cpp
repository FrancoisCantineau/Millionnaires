#include "UI/ItemSlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/DragDropOperation.h"
#include "Engine/Texture2D.h"

void UItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateBorderColor();
}

void UItemSlotWidget::InitializeSlot(UInventoryComponent* InInventory, int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
	InventoryRef = InInventory;
	
	if (InventoryRef)
	{
		ItemSlot = InventoryRef->GetSlot(SlotIndex);
		UpdateDisplay();
	}
}

void UItemSlotWidget::UpdateDisplay()
{
	if (!ItemIcon || !StackText)
	{
		return;
	}
	
	if (ItemSlot.IsEmpty())
	{
		HideSlotContent();
		return;
	}
	
	FItemData* ItemData = ItemSlot.GetItemData();
	if (!ItemData)
	{
		HideSlotContent();
		return;
	}
	
	ShowItemContent(ItemData);
}

void UItemSlotWidget::HideSlotContent()
{
	ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	StackText->SetVisibility(ESlateVisibility::Hidden);
}

void UItemSlotWidget::ShowItemContent(const FItemData* ItemData)
{
	SetItemIcon(ItemData->Icon);
	ItemIcon->SetVisibility(ESlateVisibility::Visible);
	
	UpdateStackText();
}

void UItemSlotWidget::UpdateStackText()
{
	if (ItemSlot.StackAmount > 1)
	{
		StackText->SetText(FText::AsNumber(ItemSlot.StackAmount));
		StackText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		StackText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UItemSlotWidget::SetItemSlot(const FItemSlot& NewSlot)
{
	ItemSlot = NewSlot;
	UpdateDisplay();
}

void UItemSlotWidget::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
	UpdateBorderColor();
}

FReply UItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		return HandleRightClick();
	}
	
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return HandleLeftClick(InMouseEvent);
	}
	
	return FReply::Unhandled();
}

FReply UItemSlotWidget::HandleRightClick()
{
	if (InventoryRef && !ItemSlot.IsEmpty())
	{
		InventoryRef->SplitStack(SlotIndex);
	}
	return FReply::Handled();
}

FReply UItemSlotWidget::HandleLeftClick(const FPointerEvent& InMouseEvent)
{
	OnSlotClicked.Broadcast(SlotIndex);
	
	if (!ItemSlot.IsEmpty())
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	
	return FReply::Handled();
}

void UItemSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	bIsHovered = true;
	UpdateBorderColor();
}

void UItemSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	bIsHovered = false;
	UpdateBorderColor();
}

void UItemSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
	if (ItemSlot.IsEmpty())
	{
		return;
	}
	
	OutOperation = CreateDragOperation();
}

UDragDropOperation* UItemSlotWidget::CreateDragOperation()
{
	UDragDropOperation* DragOp = NewObject<UDragDropOperation>();
	if (DragOp)
	{
		DragOp->Payload = this;
		DragOp->DefaultDragVisual = this;
		DragOp->Pivot = EDragPivot::CenterCenter;
	}
	return DragOp;
}

bool UItemSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	UpdateBorderColor();
	
	if (!InOperation || !InOperation->Payload)
	{
		return false;
	}
	
	UItemSlotWidget* SourceWidget = Cast<UItemSlotWidget>(InOperation->Payload);
	if (!SourceWidget || SourceWidget->SlotIndex == SlotIndex || !InventoryRef)
	{
		return false;
	}
	
	return InventoryRef->MoveItem(SourceWidget->SlotIndex, SlotIndex);
}

void UItemSlotWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	SetBorderColor(DragOverColor);
}

void UItemSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	UpdateBorderColor();
}

void UItemSlotWidget::SetBorderColor(const FLinearColor& Color)
{
	if (SlotBorder)
	{
		SlotBorder->SetBrushColor(Color);
	}
}

void UItemSlotWidget::UpdateBorderColor()
{
	if (bIsSelected)
	{
		SetBorderColor(SelectedColor);
	}
	else if (bIsHovered)
	{
		SetBorderColor(HoveredColor);
	}
	else
	{
		SetBorderColor(NormalColor);
	}
}

void UItemSlotWidget::SetItemIcon(TSoftObjectPtr<UTexture2D> IconPtr)
{
	if (!ItemIcon || IconPtr.IsNull())
	{
		if (ItemIcon)
		{
			ItemIcon->SetVisibility(ESlateVisibility::Hidden);
		}
		return;
	}

	if (UTexture2D* Texture = IconPtr.LoadSynchronous())
	{
		ItemIcon->SetBrushFromTexture(Texture, true);
		ItemIcon->SetVisibility(ESlateVisibility::Visible);
	}
}