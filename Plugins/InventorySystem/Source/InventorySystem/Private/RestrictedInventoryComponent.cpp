// RestrictedInventoryComponent.cpp
#include "RestrictedInventoryComponent.h"
#include "ItemDefinition.h"

bool URestrictedInventoryComponent::CanAcceptItem(const UItemDefinition* Item) const
{
	if (!Item)
	{
		return false;
	}

	if (Categories.IsEmpty())
	{
		return true;
	}

	const bool bHasAnyMatch = Item->Categories.HasAny(Categories);
	return bWhitelistMode ? bHasAnyMatch : !bHasAnyMatch;
}

bool URestrictedInventoryComponent::IsItemAllowed(const UItemDefinition* Item) const
{
	return Super::IsItemAllowed(Item) && CanAcceptItem(Item);
}
