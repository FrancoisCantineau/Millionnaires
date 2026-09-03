// ContainerSlotWidgetBase.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTypes.h"
#include "ContainerSlotWidgetBase.generated.h"

class UTextBlock;
class UImage;

/**
 * One row in a container's item list. Name a TextBlock "ItemNameText", a TextBlock
 * "QuantityText", and an Image "IconImage" in your Widget Blueprint's designer and they fill
 * in automatically — override SetupSlot/SetHighlighted in Blueprint for custom behavior.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class INVENTORYSYSTEM_API UContainerSlotWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory")
	void SetupSlot(const FInventorySlot& InSlot, int32 InSlotIndex);
	virtual void SetupSlot_Implementation(const FInventorySlot& InSlot, int32 InSlotIndex);

	/** Called by the owning ContainerWidgetBase when this slot becomes the highlighted one (or stops being). */
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory")
	void SetHighlighted(bool bHighlighted);
	virtual void SetHighlighted_Implementation(bool bHighlighted);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetSlotIndex() const { return CachedSlotIndex; }

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> QuantityText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> IconImage;

private:
	int32 CachedSlotIndex = INDEX_NONE;
};
