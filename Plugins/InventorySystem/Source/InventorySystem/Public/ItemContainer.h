// ItemContainer.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ContainerWidgetBase.h"
#include "ItemContainer.generated.h"

class UInventoryComponent;
class UItemDefinition;

/**
 * A world container — locker, drawer, chest, anything the player opens to store/retrieve items.
 * Just an Actor hosting a UInventoryComponent, plus optional lock/key state and open/close
 * bookkeeping. Wire Open()/Close()/TryUnlock() to your own interaction system.
 */
UCLASS()
class INVENTORYSYSTEM_API AItemContainer : public AActor
{
	GENERATED_BODY()

public:
	AItemContainer();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Container")
	TObjectPtr<UInventoryComponent> Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	bool bIsLocked = false;

	/** If set, TryUnlock only succeeds if the unlocker's inventory has this item. Irrelevant if bIsLocked is false. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container", meta = (EditCondition = "bIsLocked"))
	TObjectPtr<UItemDefinition> RequiredKey;

	/** If true, the key is removed from the unlocker's inventory on a successful unlock. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container", meta = (EditCondition = "bIsLocked"))
	bool bConsumeKeyOnUnlock = false;

	/** Checks UnlockerInventory for RequiredKey and unlocks if found. Returns true immediately
	 *  (no-op) if the container wasn't locked in the first place. */
	UFUNCTION(BlueprintCallable, Category = "Container")
	bool TryUnlock(UInventoryComponent* UnlockerInventory);

	/** If set, Open() creates and shows an instance of this automatically (assigning Container,
	 *  calling RefreshSlots, adding to viewport) and Close() destroys it. Leave unset to handle
	 *  the widget entirely yourself, or to use no widget at all. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container|UI")
	TSubclassOf<UContainerWidgetBase> WidgetClass;

	UFUNCTION(BlueprintPure, Category = "Container")
	UContainerWidgetBase* GetActiveWidget() const { return ActiveWidget; }

	/** No-op if locked or already open. */
	UFUNCTION(BlueprintCallable, Category = "Container")
	void Open();

	UFUNCTION(BlueprintCallable, Category = "Container")
	void Close();

	UFUNCTION(BlueprintPure, Category = "Container")
	bool IsOpen() const { return bIsOpen; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Container")
	void OnOpened();

	UFUNCTION(BlueprintImplementableEvent, Category = "Container")
	void OnClosed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Container")
	void OnUnlocked();

	/** Fired when TryUnlock is called on a locked container without the right key. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Container")
	void OnUnlockFailed();

	/** Transfers everything in SlotIndex to TakerInventory. Returns how many units were moved
	 *  (0 if the slot was empty, references missing, or the destination had no room). */
	UFUNCTION(BlueprintCallable, Category = "Container")
	int32 TakeSlot(int32 SlotIndex, UInventoryComponent* TakerInventory);

	/** Transfers every occupied slot's contents to TakerInventory. Returns the total units moved
	 *  — may be less than everything present if TakerInventory runs out of room partway through
	 *  (whatever doesn't fit safely stays in this container, nothing is lost). */
	UFUNCTION(BlueprintCallable, Category = "Container")
	int32 TakeAll(UInventoryComponent* TakerInventory);

private:
	bool bIsOpen = false;

	UPROPERTY(Transient)
	TObjectPtr<UContainerWidgetBase> ActiveWidget;
};
