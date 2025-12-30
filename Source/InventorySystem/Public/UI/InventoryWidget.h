#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UInventoryComponent;
class UItemSlotWidget;
class UUniformGridPanel;
struct FItemSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotSelectedSignature, int32, SlotIndex, UInventoryComponent*, InventoryComp);

/**
 * A UserWidget that displays an inventory grid and manages item slots.
 */
UCLASS()
class INVENTORYSYSTEM_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* SlotGrid;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 NumColumns = 5;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	UInventoryComponent* InventoryComponent;

	UPROPERTY()
	TArray<UItemSlotWidget*> SlotWidgets;
	
	bool bIsInitialized = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 CurrentlySelectedSlotIndex = -1;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitializeInventory(UInventoryComponent* InInventory);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OnSlotClicked(int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetSelectedSlotIndex() const { return CurrentlySelectedSlotIndex; }
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSlotSelectedSignature OnSlotSelected;

protected:
	void CreateSlotWidgets();
	void RefreshAllSlots();
	void RefreshSingleSlot(int32 SlotIndex);
	void UpdateSlotSelection(int32 NewSelectedIndex);
	
	void BindToInventoryEvents();
	void UnbindFromInventoryEvents();
	bool IsValidForInitialization() const;
	UItemSlotWidget* CreateAndConfigureSlot(int32 SlotIndex);

	UFUNCTION()
	void OnInventoryChangedHandler();

	UFUNCTION()
	void OnSlotChangedHandler(int32 SlotIndex, const FItemSlot& NewSlot);
};