// ContainerWidgetBase.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ContainerSlotWidgetBase.h"
#include "ContainerWidgetBase.generated.h"

class UInventoryComponent;
class UPanelWidget;

/**
 * The container's item list — owns which slot is currently highlighted (navigation state lives
 * here, not on the container Actor). Name a panel (VerticalBox, etc.) "SlotsContainer" in your
 * Widget Blueprint's designer and assign SlotWidgetClass; the list auto-populates on RefreshSlots().
 *
 * This is also where you should implement InputReceiverInterface (in a Blueprint child of this
 * class) — HandleInput naturally belongs here since it needs GetSelectedSlotIndex()/MoveSelection().
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class INVENTORYSYSTEM_API UContainerWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|UI")
	TSubclassOf<UContainerSlotWidgetBase> SlotWidgetClass;

	/** Assigns Container and subscribes to it for automatic refresh — use this instead of
	 *  setting Container directly (the property is read-only for that reason). Also calls
	 *  RefreshSlots() immediately. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetContainer(UInventoryComponent* NewContainer);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UInventoryComponent* GetContainer() const { return Container; }

	/** Rebuilds the slot list from Container->GetSlots(). Called automatically by SetContainer
	 *  and whenever the container's contents change — you shouldn't normally need to call this
	 *  yourself. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefreshSlots();

	/** Moves the highlighted slot by Delta (e.g. -1 or +1), wrapping around at the ends. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void MoveSelection(int32 Delta);

	/** The REAL inventory slot index behind the currently highlighted row — use this (not a raw
	 *  list position) when calling TakeSlot, since empty slots are skipped in the displayed list. */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetSelectedSlotIndex() const;

protected:
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> SlotsContainer;

private:
	UPROPERTY(Transient)
	TObjectPtr<UInventoryComponent> Container;

	UFUNCTION()
	void HandleInventoryChanged();

	/** Position within the COMPACTED (occupied-only) list — not a raw inventory slot index. */
	int32 SelectedIndex = 0;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UContainerSlotWidgetBase>> SlotWidgets;

	/** Maps a position in SlotWidgets/the compacted list back to its real inventory slot index. */
	TArray<int32> DisplayedSlotIndices;

	void UpdateHighlight();
};
