#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"

#include "ItemSlot.generated.h"

#pragma region ItemSlot Struct

/**
 * Represents a single slot in an inventory containing an item and its stack amount.
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FItemSlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FDataTableRowHandle ItemHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "1"))
    int32 StackAmount;

    FItemSlot() : StackAmount(0) {}
    
    FItemSlot(const FDataTableRowHandle& InItemHandle, int32 InStackAmount)
        : ItemHandle(InItemHandle), StackAmount(InStackAmount) {}

    FORCEINLINE bool IsEmpty() const 
    { 
        return ItemHandle.IsNull() || StackAmount <= 0; 
    }

    FORCEINLINE FItemData* GetItemData() const
    {
        return ItemHandle.IsNull() ? nullptr : ItemHandle.GetRow<FItemData>(TEXT("ItemSlot"));
    }

    void Clear()
    {
        ItemHandle = FDataTableRowHandle();
        StackAmount = 0;
    }

    FORCEINLINE bool IsSameItem(const FItemSlot& Other) const
    {
        return ItemHandle == Other.ItemHandle;
    }

    FORCEINLINE bool CanStackWith(const FItemSlot& Other) const
    {
        if (!IsSameItem(Other) || IsEmpty() || Other.IsEmpty())
        {
            return false;
        }

        const FItemData* ItemData = GetItemData();
        return ItemData && ItemData->bIsStackable;
    }

    FORCEINLINE int32 GetRemainingStackSpace() const
    {
        const FItemData* ItemData = GetItemData();
        if (!ItemData || !ItemData->bIsStackable)
        {
            return 0;
        }
        
        return FMath::Max(0, ItemData->MaxStackSize - StackAmount);
    }

    FORCEINLINE bool IsFullStack() const
    {
        return GetRemainingStackSpace() == 0;
    }

    bool operator==(const FItemSlot& Other) const
    {
        return ItemHandle == Other.ItemHandle && StackAmount == Other.StackAmount;
    }

    bool operator!=(const FItemSlot& Other) const
    {
        return !(*this == Other);
    }
};

#pragma endregion

#pragma region ItemSlotLibrary Class

/**
 * Blueprint function library for ItemSlot operations
 */
UCLASS()
class INVENTORYSYSTEM_API UItemSlotLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    
    /** Breaks an ItemSlot into its components */
    UFUNCTION(BlueprintPure, Category = "Inventory/ItemSlot", meta = (NativeBreakFunc))
    static void BreakItemSlot(const FItemSlot& ItemSlot, FDataTableRowHandle& ItemHandle, int32& StackAmount)
    {
        ItemHandle = ItemSlot.ItemHandle;
        StackAmount = ItemSlot.StackAmount;
    }

    /** Creates an ItemSlot from its components */
    UFUNCTION(BlueprintPure, Category = "Inventory/ItemSlot", meta = (NativeMakeFunc))
    static FItemSlot MakeItemSlot(const FDataTableRowHandle& ItemHandle, int32 StackAmount)
    {
        return FItemSlot(ItemHandle, StackAmount);
    }

    /** Checks if the ItemSlot is empty */
    UFUNCTION(BlueprintPure, Category = "Inventory/ItemSlot")
    static bool IsItemSlotEmpty(const FItemSlot& ItemSlot)
    {
        return ItemSlot.IsEmpty();
    }

    /** Retrieves the ItemData from the ItemSlot */
    UFUNCTION(BlueprintPure, Category = "Inventory/ItemSlot")
    static bool GetItemDataFromSlot(const FItemSlot& ItemSlot, FItemData& OutItemData)
    {
        if (FItemData* ItemData = ItemSlot.GetItemData())
        {
            OutItemData = *ItemData;
            return true;
        }
        return false;
    }

    /** Clears the ItemSlot */
    UFUNCTION(BlueprintCallable, Category = "Inventory/ItemSlot")
    static void ClearItemSlot(UPARAM(ref) FItemSlot& ItemSlot)
    {
        ItemSlot.Clear();
    }

    /** Checks if two ItemSlots can be stacked together */
    UFUNCTION(BlueprintPure, Category = "Inventory/ItemSlot")
    static bool CanStackSlots(const FItemSlot& SlotA, const FItemSlot& SlotB)
    {
        return SlotA.CanStackWith(SlotB);
    }

    /** Gets the remaining stack space in the ItemSlot */
    UFUNCTION(BlueprintPure, Category = "Inventory/ItemSlot")
    static int32 GetRemainingStackSpace(const FItemSlot& ItemSlot)
    {
        return ItemSlot.GetRemainingStackSpace();
    }

    /** Checks if the ItemSlot is a full stack */
    UFUNCTION(BlueprintPure, Category = "Inventory/ItemSlot")
    static bool IsFullStack(const FItemSlot& ItemSlot)
    {
        return ItemSlot.IsFullStack();
    }

    /** Checks if two ItemSlots contain the same item */
    UFUNCTION(BlueprintPure, Category = "Inventory/ItemSlot")
    static bool IsSameItem(const FItemSlot& SlotA, const FItemSlot& SlotB)
    {
        return SlotA.IsSameItem(SlotB);
    }
    
};

#pragma endregion 