#include "InventoryWidget.h"
#include "Components/UniformGridSlot.h"
#include "InventoryComponent.h"
#include "ItemSlotWidget.h"
#include "Components/UniformGridPanel.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (!IsValidForInitialization())
    {
        return;
    }
    
    if (!InventoryComponent)
    {
        APawn* OwnerPawn = GetOwningPlayerPawn();
        if (OwnerPawn)
        {
            InventoryComponent = OwnerPawn->FindComponentByClass<UInventoryComponent>();
        }
    }
    
    if (InventoryComponent)
    {
        InitializeInventory(InventoryComponent);
    }
}

void UInventoryWidget::InitializeInventory(UInventoryComponent* InInventory)
{
    if (!InInventory || (bIsInitialized && InventoryComponent == InInventory))
    {
        return;
    }
    
    UnbindFromInventoryEvents();
    
    InventoryComponent = InInventory;
    
    BindToInventoryEvents();
    CreateSlotWidgets();
    
    bIsInitialized = true;
}

void UInventoryWidget::CreateSlotWidgets()
{
    if (!IsValidForInitialization() || !InventoryComponent)
    {
        return;
    }
    
    const int32 NumSlots = InventoryComponent->GetNumSlots();
    SlotWidgets.Reserve(NumSlots);
    
    for (int32 i = 0; i < NumSlots; ++i)
    {
        if (UItemSlotWidget* SlotWidget = CreateAndConfigureSlot(i))
        {
            SlotWidgets.Add(SlotWidget);
        }
    }
    
    RefreshAllSlots();
}

UItemSlotWidget* UInventoryWidget::CreateAndConfigureSlot(int32 SlotIndex)
{
    UItemSlotWidget* SlotWidget = CreateWidget<UItemSlotWidget>(this, ItemSlotWidgetClass);
    if (!SlotWidget)
    {
        return nullptr;
    }
    
    const int32 Row = SlotIndex / NumColumns;
    const int32 Col = SlotIndex % NumColumns;
    
    UUniformGridSlot* GridSlot = SlotGrid->AddChildToUniformGrid(SlotWidget, Row, Col);
    if (!GridSlot)
    {
        return nullptr;
    }
    
    GridSlot->SetHorizontalAlignment(HAlign_Fill);
    GridSlot->SetVerticalAlignment(VAlign_Fill);
    
    SlotWidget->InitializeSlot(InventoryComponent, SlotIndex);
    SlotWidget->OnSlotClicked.AddDynamic(this, &UInventoryWidget::OnSlotClicked);
    
    return SlotWidget;
}

void UInventoryWidget::RefreshAllSlots()
{
    if (!InventoryComponent)
    {
        return;
    }
    
    for (int32 i = 0; i < SlotWidgets.Num(); ++i)
    {
        RefreshSingleSlot(i);
    }
}

void UInventoryWidget::RefreshSingleSlot(int32 SlotIndex)
{
    if (!InventoryComponent || !SlotWidgets.IsValidIndex(SlotIndex) || !SlotWidgets[SlotIndex])
    {
        return;
    }
    
    const FItemSlot SlotData = InventoryComponent->GetSlot(SlotIndex);
    SlotWidgets[SlotIndex]->SetItemSlot(SlotData);
}

void UInventoryWidget::OnSlotClicked(int32 SlotIndex)
{
    if (!InventoryComponent)
    {
        return;
    }
    
    UpdateSlotSelection(SlotIndex);
    OnSlotSelected.Broadcast(SlotIndex);
}

void UInventoryWidget::UpdateSlotSelection(int32 NewSelectedIndex)
{
    if (CurrentlySelectedSlotIndex != -1 && 
        SlotWidgets.IsValidIndex(CurrentlySelectedSlotIndex) && 
        SlotWidgets[CurrentlySelectedSlotIndex])
    {
        SlotWidgets[CurrentlySelectedSlotIndex]->SetSelected(false);
    }
    
    CurrentlySelectedSlotIndex = NewSelectedIndex;
    
    if (CurrentlySelectedSlotIndex != -1 && 
        SlotWidgets.IsValidIndex(CurrentlySelectedSlotIndex) && 
        SlotWidgets[CurrentlySelectedSlotIndex])
    {
        SlotWidgets[CurrentlySelectedSlotIndex]->SetSelected(true);
    }
}

void UInventoryWidget::BindToInventoryEvents()
{
    if (!InventoryComponent)
    {
        return;
    }
    
    InventoryComponent->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::OnInventoryChangedHandler);
    InventoryComponent->OnSlotChanged.AddDynamic(this, &UInventoryWidget::OnSlotChangedHandler);
}

void UInventoryWidget::UnbindFromInventoryEvents()
{
    if (InventoryComponent && bIsInitialized)
    {
        InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &UInventoryWidget::OnInventoryChangedHandler);
        InventoryComponent->OnSlotChanged.RemoveDynamic(this, &UInventoryWidget::OnSlotChangedHandler);
    }
}

bool UInventoryWidget::IsValidForInitialization() const
{
    return SlotGrid != nullptr && ItemSlotWidgetClass != nullptr;
}

void UInventoryWidget::OnInventoryChangedHandler()
{
    RefreshAllSlots();
}

void UInventoryWidget::OnSlotChangedHandler(int32 SlotIndex, const FItemSlot& NewSlot)
{
    RefreshSingleSlot(SlotIndex);
}