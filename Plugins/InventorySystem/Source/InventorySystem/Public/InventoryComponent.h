// InventoryComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryTypes.h"
#include "InventoryComponent.generated.h"

class UItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChangedMulticast);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotChangedMulticast, int32, SlotIndex, const FInventorySlot&, NewSlot);

/**
 * Holds a FIXED number of item slots (NumSlots) — empty slots stay in the array so a grid UI can
 * render them (rather than only knowing about occupied slots). Place on the PlayerState (or
 * wherever fits). Decoupled from UI — events just tell you something moved; your Blueprint
 * decides how to display it.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:
	/** Number of slots — fixed at BeginPlay from this value. Changing it afterward at runtime
	 *  has no effect (would require a resize policy for existing contents, not currently supported). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "1"))
	int32 NumSlots = 20;

	/** Contents this inventory starts with — filled in automatically at BeginPlay, no Blueprint
	 *  graph needed. Fill this in the Details panel for a locker's starting loot, a player's
	 *  starting gear, etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FInitialItemEntry> InitialItems;

	/** Adds Quantity units of Item — merges into existing Stackable slots first (respecting
	 *  MaxStackSize), then fills empty slots for the rest (Unique items always get one slot per
	 *  unit). Returns how many units were ACTUALLY added — can be less than requested if slots
	 *  are full, so always check the return value rather than assuming full success. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItem(UItemDefinition* Item, int32 Quantity = 1);

	/** Removes up to Quantity units of Item, across however many slots needed (emptied slots
	 *  become available again, not removed from the array). Returns how many were actually removed. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveItem(UItemDefinition* Item, int32 Quantity = 1);

	/** Clears an entire slot regardless of its contents. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ClearSlot(int32 SlotIndex);

	/** Removes exactly one unit from a stack at SlotIndex (clearing it if that empties the stack). */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveOneFromSlot(int32 SlotIndex);

	/** Moves FromSlot's contents into ToSlot — stacks automatically if both hold the same
	 *  Stackable item with room, otherwise swaps the two slots outright. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool MoveItem(int32 FromSlot, int32 ToSlot);

	/** Swaps two slots unconditionally, regardless of what's in them. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapSlots(int32 SlotA, int32 SlotB);

	/** Splits a stack roughly in half into the first available empty slot. Fails if SourceSlot
	 *  isn't a Stackable item with more than 1 unit, or if there's no empty slot to receive it. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SplitStack(int32 SourceSlot);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FInventorySlot GetSlot(int32 SlotIndex) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FInventorySlot>& GetSlots() const { return Slots; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsValidSlotIndex(int32 SlotIndex) const { return Slots.IsValidIndex(SlotIndex); }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemCount(const UItemDefinition* Item) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(const UItemDefinition* Item, int32 MinQuantity = 1) const;

	/** Fired after any successful change to any slot — implement UI refresh in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnInventoryChanged();

	/** Same information as OnInventoryChanged, as a real multicast delegate — use this instead
	 *  when an EXTERNAL object (not a Blueprint subclass of this component) needs to react, e.g.
	 *  UContainerWidgetBase::SetContainer subscribes to this for automatic refresh. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChangedMulticast OnInventoryChangedDelegate;

	/** Fired for the SPECIFIC slot that changed — use this instead of OnInventoryChanged to
	 *  refresh only the one UI element that needs it, rather than the whole grid. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnSlotChanged(int32 SlotIndex, const FInventorySlot& NewSlot);

	/** Multicast-delegate companion to OnSlotChanged, for external subscribers. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSlotChangedMulticast OnSlotChangedDelegate;

	/** Self-contained save/restore. Call these from your project's save bridge. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FInventorySaveData CaptureSaveData() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RestoreSaveData(const FInventorySaveData& SaveData);

protected:
	/** Override to reject certain items from this inventory (see URestrictedInventoryComponent). */
	virtual bool IsItemAllowed(const UItemDefinition* Item) const;

private:
	UPROPERTY(Transient)
	TArray<FInventorySlot> Slots;

	int32 FindEmptySlot() const;
	void NotifySlotChanged(int32 SlotIndex);
};
