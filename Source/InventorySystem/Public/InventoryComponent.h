#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryInterface.h" 
#include "ItemSlot.h"

#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotChanged, int32, SlotIndex, const FItemSlot&, NewSlot);

/**
 * Component that manages an inventory with slots
 * Supports adding, removing, moving, stacking, splitting, and sorting items
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UInventoryComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

public:
    
    UInventoryComponent();
    
    void BeginPlay();
    
    /* Interface Implementations */
    virtual int32 AddItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount) override;
    virtual bool HasSpaceForItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount) const override;
    
#pragma region Events
    
    /** Event called when any slot in inventory changes */
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryChanged OnInventoryChanged;

    /** Event called when a specific slot changes */
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnSlotChanged OnSlotChanged;
    
#pragma endregion

#pragma region Properties
    
    /** Number of slots in this inventory */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "1"))
    int32 NumSlots = 20;

protected:
    
    /** Array of item slots - uses TArray for cache-friendly iteration */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FItemSlot> ItemSlots;

#pragma endregion

#pragma region Core Operations
    
public:
    // ENLÈVE CES DEUX LIGNES - elles sont déjà déclarées via l'interface
    // UFUNCTION(BlueprintCallable, Category = "Inventory")
    // int32 AddItem(const FDataTableRowHandle& ItemHandle, int32 Amount);
    
    // UFUNCTION(BlueprintCallable, Category = "Inventory")
    // bool HasSpaceForItem(const FDataTableRowHandle& ItemHandle, int32 Amount) const;

    /** Remove all items from a specific slot */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(int32 SlotIndex);

    /** Move item from one slot to another with automatic stacking/swapping */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool MoveItem(int32 FromSlot, int32 ToSlot);

    /** Swap items between two slots unconditionally */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool SwapItems(int32 SlotA, int32 SlotB);

    /** Split stack in half, placing half in nearest empty slot */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool SplitStack(int32 SourceSlot);

#pragma endregion

#pragma region Query Operations
    
    /** Get slot data at specific index */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    FItemSlot GetSlot(int32 SlotIndex) const;

    /** Get reference to all slots for iteration */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    const TArray<FItemSlot>& GetAllSlots() const { return ItemSlots; }

    /** Get total number of slots in inventory */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetNumSlots() const { return NumSlots; }
    
    /** Validate if slot index is within bounds */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool IsValidSlotIndex(int32 SlotIndex) const;
    
#pragma endregion

#pragma region Internal Helpers
    
protected:

    /** Initialize inventory slots on begin play */
    void InitializeSlots();

    /** Check if item is allowed in this inventory */
    virtual bool IsItemAllowed(const FItemData* ItemData) const;

    /** Find first empty slot index */
    int32 FindEmptySlot() const;

    /** Find all slots that can stack with given item */
    TArray<int32> FindStackableSlots(const FDataTableRowHandle& ItemHandle) const;

    /** Attempt to stack items in existing slots */
    int32 TryStackItem(const FDataTableRowHandle& ItemHandle, int32 Amount);

    /** Attempt to place items in empty slots */
    int32 TryPlaceInEmptySlots(const FDataTableRowHandle& ItemHandle, int32 Amount);

    /** Broadcast slot change event */
    void NotifySlotChanged(int32 SlotIndex);

    /** Broadcast inventory change event */
    void NotifyInventoryChanged() const;
    
#pragma endregion
};