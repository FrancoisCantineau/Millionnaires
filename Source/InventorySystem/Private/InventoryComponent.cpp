#include "InventoryComponent.h"

#pragma region Lifecycle

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
    InitializeSlots();
}

void UInventoryComponent::InitializeSlots()
{
    ItemSlots.Empty();
    ItemSlots.SetNum(NumSlots);
}

#pragma endregion

#pragma region Core Operations

/*
 * Adds items to the inventory, stacking where possible.
 */
int32 UInventoryComponent::AddItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount)
{
    if (ItemHandle.IsNull() || Amount <= 0)
    {
        return Amount; 
    }
    
    const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("AddItem"));
    if (!ItemData)
    {
        return Amount;
    }
    
    if (!IsItemAllowed(ItemData))
    {
        return Amount;
    }

    int32 Remaining = Amount;

    // Try to stack into existing slots
    if (ItemData->bIsStackable)
    {
        Remaining = TryStackItem(ItemHandle, Remaining);
    }

    // Use empty slots only if stacking didn't fit everything
    if (Remaining > 0)
    {
        Remaining = TryPlaceInEmptySlots(ItemHandle, Remaining);
    }

    // Calculate how many items were actually added
    const int32 ItemsAdded = Amount - Remaining;
  
    // Only notify UI if something changed
    if (ItemsAdded > 0)
    {
        NotifyInventoryChanged();
    }
   
    return Remaining;
}

/*
 * Checks if there is enough space to add a certain amount of an item.
 */
bool UInventoryComponent::HasSpaceForItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount) const
{
    if (ItemHandle.IsNull() || Amount <= 0)
    {
        return false;
    }
    
    const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("HasSpaceForItem"));
    if (!ItemData)
    {
        return false;
    }
    
    if (!IsItemAllowed(ItemData))
    {
        return false;
    }

    if (!ItemData->bIsStackable)
    {
        for (const FItemSlot& Slot : ItemSlots)
        {
            if (Slot.IsEmpty())
            {
                return true;
            }
        }
        return false;
    }

    int32 RemainingToPlace = Amount;
    const int32 MaxStack = FMath::Max(1, ItemData->GetMaxStackSize());

    for (const FItemSlot& Slot : ItemSlots)
    {
        if (Slot.IsSameItem(FItemSlot(ItemHandle, 1)))
        {
            const int32 SpaceInSlot = MaxStack - Slot.StackAmount;
            RemainingToPlace -= SpaceInSlot;

            if (RemainingToPlace <= 0)
            {
                return true;
            }
        }
    }

    int32 EmptySlotsAvailable = 0;
    for (const FItemSlot& Slot : ItemSlots)
    {
        if (Slot.IsEmpty())
        {
            EmptySlotsAvailable++;
        }
    }

    const int32 EmptySlotsNeeded = (RemainingToPlace + MaxStack - 1) / MaxStack;

    return EmptySlotsAvailable >= EmptySlotsNeeded;
}

/*
 * Removes all items from a specific slot.
 */
bool UInventoryComponent::RemoveItem(int32 SlotIndex)
{
    if (!IsValidSlotIndex(SlotIndex))
    {
        return false;
    }

    if (ItemSlots[SlotIndex].IsEmpty())
    {
        return false;
    }

    ItemSlots[SlotIndex].Clear();
    NotifySlotChanged(SlotIndex);
    return true;
}

/*
 * Removes one item from a stack in a specific slot.
 */
bool UInventoryComponent::RemoveOneFromStack(int32 SlotIndex)
{
    if (!IsValidSlotIndex(SlotIndex))
    {
        return false;
    }

    if (ItemSlots[SlotIndex].IsEmpty())
    {
        return false;
    }

    ItemSlots[SlotIndex].StackAmount--;

    if (ItemSlots[SlotIndex].StackAmount <= 0)
    {
        ItemSlots[SlotIndex].Clear();
    }

    NotifySlotChanged(SlotIndex);
    return true;
}

/*
 * Moves items between two slots, stacking if possible.
 */
bool UInventoryComponent::MoveItem(int32 FromSlot, int32 ToSlot)
{
    if (!IsValidSlotIndex(FromSlot) || !IsValidSlotIndex(ToSlot))
    {
        return false;
    }

    if (FromSlot == ToSlot)
    {
        return false;
    }

    if (ItemSlots[FromSlot].IsEmpty())
    {
        return false;
    }

    // If target slot is empty, move item directly
    if (ItemSlots[ToSlot].IsEmpty())
    {
        ItemSlots[ToSlot] = ItemSlots[FromSlot];
        ItemSlots[FromSlot].Clear();
        NotifySlotChanged(FromSlot);
        NotifySlotChanged(ToSlot);
        return true;
    }

    // Attempt to stack items if they are the same
    if (ItemSlots[FromSlot].IsSameItem(ItemSlots[ToSlot]))
    {
        const FItemData* ItemData = ItemSlots[FromSlot].GetItemData();
        if (ItemData && ItemData->bIsStackable)
        {
            const int32 SpaceInTarget = ItemData->GetMaxStackSize() - ItemSlots[ToSlot].StackAmount;
            const int32 AmountToMove = FMath::Min(ItemSlots[FromSlot].StackAmount, SpaceInTarget);

            if (AmountToMove > 0)
            {
                ItemSlots[ToSlot].StackAmount += AmountToMove;
                ItemSlots[FromSlot].StackAmount -= AmountToMove;

                if (ItemSlots[FromSlot].StackAmount <= 0)
                {
                    ItemSlots[FromSlot].Clear();
                }

                NotifySlotChanged(FromSlot);
                NotifySlotChanged(ToSlot);
                return true;
            }
        }
    }

    return SwapItems(FromSlot, ToSlot);
}

/*
 * Swaps items between two slots unconditionally.
 */
bool UInventoryComponent::SwapItems(int32 SlotA, int32 SlotB)
{
    if (!IsValidSlotIndex(SlotA) || !IsValidSlotIndex(SlotB))
    {
        return false;
    }

    if (SlotA == SlotB)
    {
        return false;
    }

    const FItemSlot Temp = ItemSlots[SlotA];
    ItemSlots[SlotA] = ItemSlots[SlotB];
    ItemSlots[SlotB] = Temp;

    NotifySlotChanged(SlotA);
    NotifySlotChanged(SlotB);
    return true;
}

/*
 * Splits a stack in half, placing half in the nearest empty slot.
 */
bool UInventoryComponent::SplitStack(int32 SourceSlot)
{
    if (!IsValidSlotIndex(SourceSlot))
    {
        return false;
    }
    
    if (ItemSlots[SourceSlot].StackAmount <= 1)
    {
        return false;
    }

    const FItemData* ItemData = ItemSlots[SourceSlot].GetItemData();
    if (!ItemData || !ItemData->bIsStackable)
    {
        return false;
    }

    const int32 EmptySlot = FindEmptySlot();
    if (EmptySlot == INDEX_NONE)
    {
        return false;
    }

    const int32 HalfAmount = ItemSlots[SourceSlot].StackAmount / 2;
    ItemSlots[SourceSlot].StackAmount -= HalfAmount;
    ItemSlots[EmptySlot] = FItemSlot(ItemSlots[SourceSlot].ItemHandle, HalfAmount);

    NotifySlotChanged(SourceSlot);
    NotifySlotChanged(EmptySlot);
    return true;
}

#pragma endregion

#pragma region Query Operations

/*
 * Gets slot data at a specific index.
 */
FItemSlot UInventoryComponent::GetSlot(int32 SlotIndex) const
{
    if (!IsValidSlotIndex(SlotIndex))
    {
        return FItemSlot();
    }

    return ItemSlots[SlotIndex];
}

/*
 * Validates if a slot index is within bounds.
 */
bool UInventoryComponent::IsValidSlotIndex(int32 SlotIndex) const
{
    return ItemSlots.IsValidIndex(SlotIndex);
}

#pragma endregion

#pragma region Internal Helpers

/*
 * Checks if an item is allowed in this inventory based on its category.
 */
bool UInventoryComponent::IsItemAllowed(const FItemData* ItemData) const
{
    return ItemData != nullptr;
}

/*
 * Finds the first empty slot index.
 */
int32 UInventoryComponent::FindEmptySlot() const
{
    for (int32 i = 0; i < ItemSlots.Num(); ++i)
    {
        if (ItemSlots[i].IsEmpty())
        {
            return i;
        }
    }

    return INDEX_NONE;
}

/*
 * Finds all slots that can stack with a given item.
 */
TArray<int32> UInventoryComponent::FindStackableSlots(const FDataTableRowHandle& ItemHandle) const
{
    TArray<int32> StackableSlots;

    const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("FindStackable"));
    if (!ItemData || !ItemData->bIsStackable)
    {
        return StackableSlots;
    }

    for (int32 i = 0; i < ItemSlots.Num(); ++i)
    {
        if (ItemSlots[i].IsSameItem(FItemSlot(ItemHandle, 1)))
        {
            if (ItemSlots[i].StackAmount < ItemData->GetMaxStackSize())
            {
                StackableSlots.Add(i);
            }
        }
    }

    return StackableSlots;
}

/*
 * Attempts to stack items in existing slots.
 */
int32 UInventoryComponent::TryStackItem(const FDataTableRowHandle& ItemHandle, int32 Amount)
{
    const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("TryStack"));
    if (!ItemData)
    {
        return Amount;
    }

    const TArray<int32> StackableSlots = FindStackableSlots(ItemHandle);
    
    int32 Remaining = Amount;

    // Stack into existing slots
    for (int32 SlotIndex : StackableSlots)
    {
        if (Remaining <= 0)
        {
            break;
        }

        const int32 SpaceInSlot = ItemData->GetMaxStackSize() - ItemSlots[SlotIndex].StackAmount;
        const int32 AmountToAdd = FMath::Min(Remaining, SpaceInSlot);

        ItemSlots[SlotIndex].StackAmount += AmountToAdd;
        Remaining -= AmountToAdd;

        NotifySlotChanged(SlotIndex);
    }

    return Remaining;
}

/*
 * Attempts to place items in empty slots.
 */
int32 UInventoryComponent::TryPlaceInEmptySlots(const FDataTableRowHandle& ItemHandle, int32 Amount)
{
    const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("TryPlace"));
    if (!ItemData)
    {
        return Amount;
    }

    int32 Remaining = Amount;
    const int32 MaxStack = ItemData->GetMaxStackSize();

    // Place in empty slots
    for (int32 i = 0; i < ItemSlots.Num() && Remaining > 0; ++i)
    {
        if (ItemSlots[i].IsEmpty())
        {
            const int32 AmountToAdd = FMath::Min(Remaining, MaxStack);
            
            ItemSlots[i] = FItemSlot(ItemHandle, AmountToAdd);
            Remaining -= AmountToAdd;

            NotifySlotChanged(i);
        }
    }

    return Remaining;
}

/*
 * Broadcasts a slot change event.
 */
void UInventoryComponent::NotifySlotChanged(int32 SlotIndex)
{
    if (IsValidSlotIndex(SlotIndex))
    {
        OnSlotChanged.Broadcast(SlotIndex, ItemSlots[SlotIndex]);
    }
}

/*
 * Broadcasts an inventory change event.
 */
void UInventoryComponent::NotifyInventoryChanged() const
{
    OnInventoryChanged.Broadcast();
}

#pragma endregion