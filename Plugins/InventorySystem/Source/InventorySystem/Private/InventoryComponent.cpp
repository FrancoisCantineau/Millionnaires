// InventoryComponent.cpp
#include "InventoryComponent.h"
#include "ItemDefinition.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	Slots.SetNum(FMath::Max(NumSlots, 1));

	for (const FInitialItemEntry& Entry : InitialItems)
	{
		if (Entry.Item)
		{
			AddItem(Entry.Item, Entry.Quantity);
		}
	}
}

bool UInventoryComponent::IsItemAllowed(const UItemDefinition* Item) const
{
	return Item != nullptr;
}

int32 UInventoryComponent::FindEmptySlot() const
{
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].IsEmpty())
		{
			return i;
		}
	}
	return INDEX_NONE;
}

void UInventoryComponent::NotifySlotChanged(int32 SlotIndex)
{
	if (Slots.IsValidIndex(SlotIndex))
	{
		OnSlotChanged(SlotIndex, Slots[SlotIndex]);
		OnSlotChangedDelegate.Broadcast(SlotIndex, Slots[SlotIndex]);
		OnInventoryChanged();
		OnInventoryChangedDelegate.Broadcast();
	}
}

int32 UInventoryComponent::AddItem(UItemDefinition* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0 || !IsItemAllowed(Item))
	{
		return 0;
	}

	int32 Remaining = Quantity;

	if (Item->StackingType == EItemStackingType::Stackable)
	{
		for (int32 i = 0; i < Slots.Num() && Remaining > 0; ++i)
		{
			if (Slots[i].ItemDef == Item && Slots[i].Quantity < Item->MaxStackSize)
			{
				const int32 SpaceInSlot = Item->MaxStackSize - Slots[i].Quantity;
				const int32 ToAdd = FMath::Min(SpaceInSlot, Remaining);
				Slots[i].Quantity += ToAdd;
				Remaining -= ToAdd;
				NotifySlotChanged(i);
			}
		}
		while (Remaining > 0)
		{
			const int32 EmptyIndex = FindEmptySlot();
			if (EmptyIndex == INDEX_NONE)
			{
				break;
			}
			Slots[EmptyIndex].ItemDef = Item;
			Slots[EmptyIndex].Quantity = FMath::Min(Item->MaxStackSize, Remaining);
			Remaining -= Slots[EmptyIndex].Quantity;
			NotifySlotChanged(EmptyIndex);
		}
	}
	else // Unique
	{
		while (Remaining > 0)
		{
			const int32 EmptyIndex = FindEmptySlot();
			if (EmptyIndex == INDEX_NONE)
			{
				break;
			}
			Slots[EmptyIndex].ItemDef = Item;
			Slots[EmptyIndex].Quantity = 1;
			--Remaining;
			NotifySlotChanged(EmptyIndex);
		}
	}

	return Quantity - Remaining;
}

int32 UInventoryComponent::RemoveItem(UItemDefinition* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0)
	{
		return 0;
	}

	int32 Remaining = Quantity;

	for (int32 i = 0; i < Slots.Num() && Remaining > 0; ++i)
	{
		if (Slots[i].ItemDef != Item)
		{
			continue;
		}

		const int32 ToRemove = FMath::Min(Slots[i].Quantity, Remaining);
		Slots[i].Quantity -= ToRemove;
		Remaining -= ToRemove;

		if (Slots[i].Quantity <= 0)
		{
			Slots[i].Clear();
		}
		NotifySlotChanged(i);
	}

	return Quantity - Remaining;
}

bool UInventoryComponent::ClearSlot(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty())
	{
		return false;
	}

	Slots[SlotIndex].Clear();
	NotifySlotChanged(SlotIndex);
	return true;
}

bool UInventoryComponent::RemoveOneFromSlot(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty())
	{
		return false;
	}

	--Slots[SlotIndex].Quantity;
	if (Slots[SlotIndex].Quantity <= 0)
	{
		Slots[SlotIndex].Clear();
	}
	NotifySlotChanged(SlotIndex);
	return true;
}

bool UInventoryComponent::MoveItem(int32 FromSlot, int32 ToSlot)
{
	if (!Slots.IsValidIndex(FromSlot) || !Slots.IsValidIndex(ToSlot) || FromSlot == ToSlot)
	{
		return false;
	}
	if (Slots[FromSlot].IsEmpty())
	{
		return false;
	}

	if (Slots[ToSlot].IsEmpty())
	{
		Slots[ToSlot] = Slots[FromSlot];
		Slots[FromSlot].Clear();
		NotifySlotChanged(FromSlot);
		NotifySlotChanged(ToSlot);
		return true;
	}

	// Same stackable item with room -> merge as much as fits.
	if (Slots[FromSlot].ItemDef == Slots[ToSlot].ItemDef && Slots[FromSlot].ItemDef->StackingType == EItemStackingType::Stackable)
	{
		const int32 SpaceInTarget = Slots[FromSlot].ItemDef->MaxStackSize - Slots[ToSlot].Quantity;
		const int32 AmountToMove = FMath::Min(Slots[FromSlot].Quantity, SpaceInTarget);

		if (AmountToMove > 0)
		{
			Slots[ToSlot].Quantity += AmountToMove;
			Slots[FromSlot].Quantity -= AmountToMove;
			if (Slots[FromSlot].Quantity <= 0)
			{
				Slots[FromSlot].Clear();
			}
			NotifySlotChanged(FromSlot);
			NotifySlotChanged(ToSlot);
			return true;
		}
	}

	// Different items, or a full stack — fall back to swapping.
	return SwapSlots(FromSlot, ToSlot);
}

bool UInventoryComponent::SwapSlots(int32 SlotA, int32 SlotB)
{
	if (!Slots.IsValidIndex(SlotA) || !Slots.IsValidIndex(SlotB) || SlotA == SlotB)
	{
		return false;
	}

	Slots.Swap(SlotA, SlotB);
	NotifySlotChanged(SlotA);
	NotifySlotChanged(SlotB);
	return true;
}

bool UInventoryComponent::SplitStack(int32 SourceSlot)
{
	if (!Slots.IsValidIndex(SourceSlot))
	{
		return false;
	}

	FInventorySlot& Source = Slots[SourceSlot];
	if (Source.IsEmpty() || Source.Quantity <= 1 || !Source.ItemDef || Source.ItemDef->StackingType != EItemStackingType::Stackable)
	{
		return false;
	}

	const int32 EmptyIndex = FindEmptySlot();
	if (EmptyIndex == INDEX_NONE)
	{
		return false;
	}

	const int32 HalfAmount = Source.Quantity / 2;
	Source.Quantity -= HalfAmount;
	Slots[EmptyIndex].ItemDef = Source.ItemDef;
	Slots[EmptyIndex].Quantity = HalfAmount;

	NotifySlotChanged(SourceSlot);
	NotifySlotChanged(EmptyIndex);
	return true;
}

FInventorySlot UInventoryComponent::GetSlot(int32 SlotIndex) const
{
	return Slots.IsValidIndex(SlotIndex) ? Slots[SlotIndex] : FInventorySlot();
}

int32 UInventoryComponent::GetItemCount(const UItemDefinition* Item) const
{
	int32 Total = 0;
	for (const FInventorySlot& Slot : Slots)
	{
		if (Slot.ItemDef == Item)
		{
			Total += Slot.Quantity;
		}
	}
	return Total;
}

bool UInventoryComponent::HasItem(const UItemDefinition* Item, int32 MinQuantity) const
{
	return GetItemCount(Item) >= MinQuantity;
}

FInventorySaveData UInventoryComponent::CaptureSaveData() const
{
	FInventorySaveData SaveData;
	SaveData.Slots = Slots;
	return SaveData;
}

void UInventoryComponent::RestoreSaveData(const FInventorySaveData& SaveData)
{
	Slots = SaveData.Slots;
	Slots.SetNum(FMath::Max(NumSlots, Slots.Num()));
	OnInventoryChanged();
}
