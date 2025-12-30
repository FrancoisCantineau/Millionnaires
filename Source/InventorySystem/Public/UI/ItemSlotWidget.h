#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ItemSlot.h"
#include "InventoryComponent.h"
#include "ItemSlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotClickedSignature, int32, SlotIndex);

/**
 * Widget representing a single inventory slot
 * Handles display and drag/drop interactions
 */
UCLASS()
class INVENTORYSYSTEM_API UItemSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    
    UPROPERTY(meta = (BindWidget))
    UBorder* SlotBorder;

    UPROPERTY(meta = (BindWidget))
    UImage* ItemIcon;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* StackText;

    // === Properties ===

    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    int32 SlotIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    FItemSlot ItemSlot;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    UInventoryComponent* InventoryRef;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    bool bIsSelected = false;

    // === Colors for visual feedback ===

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Style")
    FLinearColor NormalColor = FLinearColor(0.1f, 0.1f, 0.1f, 0.8f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Style")
    FLinearColor HoveredColor = FLinearColor(0.2f, 0.2f, 0.3f, 0.9f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Style")
    FLinearColor DragOverColor = FLinearColor(0.3f, 0.5f, 0.3f, 0.9f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Style")
    FLinearColor SelectedColor = FLinearColor(0.5f, 0.5f, 0.1f, 1.0f);

    // === Delegates ===

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnSlotClickedSignature OnSlotClicked;

    // === Core Functions ===

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void InitializeSlot(UInventoryComponent* InInventory, int32 InSlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void UpdateDisplay();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetItemSlot(const FItemSlot& NewSlot);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetSelected(bool bSelected);

protected:
    // === UUserWidget Overrides ===
    
    virtual void NativeConstruct() override;

    // === Mouse Interaction Overrides ===
    
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
    void SetBorderColor(const FLinearColor& Color);
    void SetItemIcon(TSoftObjectPtr<UTexture2D> IconPtr);
    void UpdateBorderColor();
    
    void HideSlotContent();
    void ShowItemContent(const FItemData* ItemData);
    void UpdateStackText();
    
    FReply HandleRightClick();
    FReply HandleLeftClick(const FPointerEvent& InMouseEvent);
    UDragDropOperation* CreateDragOperation();

    bool bIsHovered = false;
};