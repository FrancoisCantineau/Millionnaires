// ItemContainer.cpp
#include "ItemContainer.h"
#include "InventoryComponent.h"
#include "InventoryFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AItemContainer::AItemContainer()
{
	PrimaryActorTick.bCanEverTick = false;
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

bool AItemContainer::TryUnlock(UInventoryComponent* UnlockerInventory)
{
	if (!bIsLocked)
	{
		return true;
	}

	if (!RequiredKey || !UnlockerInventory || !UnlockerInventory->HasItem(RequiredKey, 1))
	{
		OnUnlockFailed();
		return false;
	}

	if (bConsumeKeyOnUnlock)
	{
		UnlockerInventory->RemoveItem(RequiredKey, 1);
	}

	bIsLocked = false;
	OnUnlocked();
	return true;
}

void AItemContainer::Open()
{
	if (bIsLocked || bIsOpen)
	{
		return;
	}
	bIsOpen = true;

	if (WidgetClass && !ActiveWidget)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			ActiveWidget = CreateWidget<UContainerWidgetBase>(PC, WidgetClass);
			if (ActiveWidget)
			{
				ActiveWidget->SetContainer(Inventory);
				ActiveWidget->AddToViewport();
			}
		}
	}

	OnOpened();
}

void AItemContainer::Close()
{
	if (!bIsOpen)
	{
		return;
	}
	bIsOpen = false;

	if (ActiveWidget)
	{
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}

	OnClosed();
}

int32 AItemContainer::TakeSlot(int32 SlotIndex, UInventoryComponent* TakerInventory)
{
	if (!Inventory || !TakerInventory)
	{
		return 0;
	}

	const FInventorySlot Slot = Inventory->GetSlot(SlotIndex);
	if (Slot.IsEmpty())
	{
		return 0;
	}

	return UInventoryFunctionLibrary::TransferItem(Inventory, TakerInventory, Slot.ItemDef, Slot.Quantity);
}

int32 AItemContainer::TakeAll(UInventoryComponent* TakerInventory)
{
	if (!Inventory || !TakerInventory)
	{
		return 0;
	}

	// Snapshot first — transferring changes slot contents mid-loop, and we're iterating by index.
	const TArray<FInventorySlot> SlotsSnapshot = Inventory->GetSlots();

	int32 TotalTaken = 0;
	for (const FInventorySlot& Slot : SlotsSnapshot)
	{
		if (!Slot.IsEmpty())
		{
			TotalTaken += UInventoryFunctionLibrary::TransferItem(Inventory, TakerInventory, Slot.ItemDef, Slot.Quantity);
		}
	}
	return TotalTaken;
}
