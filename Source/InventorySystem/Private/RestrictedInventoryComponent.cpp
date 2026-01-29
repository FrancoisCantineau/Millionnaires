#include "RestrictedInventoryComponent.h"

#pragma region Public Methods

/**
* Checks if the given item can be accepted based on category restrictions
*/
bool URestrictedInventoryComponent::CanAcceptItem(const FDataTableRowHandle& ItemHandle) const
{
    const FItemData* ItemData = GetItemDataSafe(ItemHandle, TEXT("CanAcceptItem"));
    if (!ItemData) 
    {
        return false;
    }

    bool bResult = PassesRestrictions(ItemData);
   
    return bResult;
}

/**
 * Checks if the given category is allowed
 */
bool URestrictedInventoryComponent::IsCategoryAllowed(EItemCategory Category) const
{
    return CategoryFilter.IsAllowed(Category);
}

#pragma endregion

#pragma region Overrides

/**
 * Override to enforce item restrictions
 */
bool URestrictedInventoryComponent::IsItemAllowed(const FItemData* ItemData) const
{
    if (!ItemData)
    {
        return false;
    }

    return PassesRestrictions(ItemData);
}

/**
 * Override to enforce item restrictions when adding items
 */
int32 URestrictedInventoryComponent::AddItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount)
{
    if (Amount <= 0) 
    {
        return Amount;
    }

    const FItemData* ItemData = GetItemDataSafe(ItemHandle, TEXT("AddItem"));
    if (!ItemData) 
    {
        return Amount;
    }

    if (!PassesRestrictions(ItemData)) 
    {
        return Amount;
    }

    return Super::AddItem_Implementation(ItemHandle, Amount);
}

/**
 * Override to enforce item restrictions when checking for space
 */
bool URestrictedInventoryComponent::HasSpaceForItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount) const
{
    if (Amount <= 0) 
    {
        return false;
    }

    const FItemData* ItemData = GetItemDataSafe(ItemHandle, TEXT("HasSpaceForItem"));
    if (!ItemData) 
    {
        return false;
    }

    if (!PassesRestrictions(ItemData)) 
    {
        return false;
    }

    return Super::HasSpaceForItem_Implementation(ItemHandle, Amount);
}

#pragma endregion

#pragma region Helpers

/**
 * Helper function to check if an item passes the category restrictions
 */
bool URestrictedInventoryComponent::PassesRestrictions(const FItemData* ItemData) const
{
    if (!ItemData) 
    {
        return false;
    }

    return IsCategoryAllowed(ItemData->Category);
}

/**
 * Helper function to safely get item data from handle
 */
const FItemData* URestrictedInventoryComponent::GetItemDataSafe(const FDataTableRowHandle& ItemHandle, const FString& Context) const
{
    if (ItemHandle.IsNull()) 
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: ItemHandle is null"), *Context);
        return nullptr;
    }

    const FItemData* ItemData = ItemHandle.GetRow<FItemData>(*Context);
    if (!ItemData) 
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: Could not get ItemData"), *Context);
        return nullptr;
    }

    return ItemData;
}

#pragma endregion
