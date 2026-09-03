// InventoryFunctionLibrary.cpp
#include "InventoryFunctionLibrary.h"
#include "InventoryComponent.h"

int32 UInventoryFunctionLibrary::TransferItem(UInventoryComponent* Source, UInventoryComponent* Destination, UItemDefinition* Item, int32 Quantity)
{
	if (!Source || !Destination || !Item || Quantity <= 0)
	{
		return 0;
	}

	const int32 Removed = Source->RemoveItem(Item, Quantity);
	if (Removed <= 0)
	{
		return 0;
	}

	const int32 Added = Destination->AddItem(Item, Removed);
	if (Added < Removed)
	{
		// Destination couldn't take it all — put the leftover back in Source rather than losing it.
		Source->AddItem(Item, Removed - Added);
	}

	return Added;
}
