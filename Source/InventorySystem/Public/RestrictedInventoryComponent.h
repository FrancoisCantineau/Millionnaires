#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "RestrictedInventoryComponent.generated.h"

#pragma region ItemCategoryFilter

/**
 * Small struct to handle category-based filtering (Whitelist/Blacklist)
 */
USTRUCT(BlueprintType)
struct FItemCategoryFilter
{
    GENERATED_BODY()

    /** Categories that are restricted or allowed based on bWhitelistMode */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Filter")
    TArray<EItemCategory> Categories;

    /** If true, only items in Categories are allowed; if false, items in Categories are disallowed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Filter")
    bool bWhitelistMode = true;

    bool IsAllowed(EItemCategory Category) const
    {
        if (Categories.Num() == 0)
        {
            return true;
        }

        bool bInList = Categories.Contains(Category);
        return bWhitelistMode ? bInList : !bInList;
    }
};

#pragma endregion 

/**
 * Extension of UInventoryComponent that adds item category restrictions
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API URestrictedInventoryComponent : public UInventoryComponent
{
    GENERATED_BODY()

public:

#pragma region Properties
    
    /** Filter for allowed/restricted item categories */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Restrictions")
    FItemCategoryFilter CategoryFilter;
    
#pragma endregion

#pragma region Public Methods
    
    /** Checks if the given item can be accepted based on category restrictions */
    UFUNCTION(BlueprintPure, Category = "Inventory|Restrictions")
    bool CanAcceptItem(const FDataTableRowHandle& ItemHandle) const;

    /** Checks if the given category is allowed */
    UFUNCTION(BlueprintPure, Category = "Inventory|Restrictions")
    bool IsCategoryAllowed(EItemCategory Category) const;
    
#pragma endregion

protected:

#pragma region Overrides
    
    /** Override to enforce item restrictions */
    virtual bool IsItemAllowed(const FItemData* ItemData) const override;

    /** Override to enforce item restrictions on adding items */
    virtual int32 AddItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount) override;

    /** Override to enforce item restrictions on checking space */
    virtual bool HasSpaceForItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount) const override;
    
#pragma endregion

private:

#pragma region Helpers
    
    /** Helper function to check if an item passes the category restrictions */
    bool PassesRestrictions(const FItemData* ItemData) const;

    /** Helper function to safely get item data from handle */
    const FItemData* GetItemDataSafe(const FDataTableRowHandle& ItemHandle, const FString& Context) const;
    
#pragma endregion
    
};
