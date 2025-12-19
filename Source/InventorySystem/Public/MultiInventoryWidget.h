#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiInventoryWidget.generated.h"

class UInventoryWidget;
class URestrictedInventoryComponent;

/**
 * Widget that manages multiple inventory widgets (e.g., equipment, consumables, general)
 */
UCLASS()
class INVENTORYSYSTEM_API UMultiInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

#pragma region Properties
    
protected:
    
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    URestrictedInventoryComponent* EquipmentInventoryComponent;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    URestrictedInventoryComponent* ConsumableInventoryComponent;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    UInventoryComponent* GeneralInventoryComponent;

public:
    
    UPROPERTY(meta = (BindWidget))
    UInventoryWidget* EquipmentInventory;

    UPROPERTY(meta = (BindWidget))
    UInventoryWidget* ConsumableInventory;

    UPROPERTY(meta = (BindWidget))
    UInventoryWidget* GeneralInventory;
    
#pragma endregion

protected:
    
    virtual void NativeConstruct() override;

#pragma region Public Methods
    
public:
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void InitializeAllInventories(
        URestrictedInventoryComponent* InEquipmentInventory,
        URestrictedInventoryComponent* InConsumableInventory,
        UInventoryComponent* InGeneralInventory
    );

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AutoFindInventoryComponents();
    
#pragma endregion

protected:
    
    void SetupInventoryWidget(UInventoryWidget* Widget, UInventoryComponent* Component);
    URestrictedInventoryComponent* FindInventoryByTag(const FName& Tag) const;
    
};
