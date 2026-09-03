// ItemDefinition.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ItemDefinition.generated.h"

class UTexture2D;

UENUM(BlueprintType)
enum class EItemStackingType : uint8
{
	/** Never stacks — every unit occupies its own slot (weapons, key items, unique tools). */
	Unique,
	/** Multiple units share one slot up to MaxStackSize (ammo, consumables, currency-like items). */
	Stackable
};

/**
 * Defines a kind of item — author these as assets, reference them everywhere (inventory slots,
 * shop listings, pickups). Pure data, no gameplay logic.
 */
UCLASS(BlueprintType)
class INVENTORYSYSTEM_API UItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemStackingType StackingType = EItemStackingType::Unique;

	/** Only relevant when StackingType is Stackable. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (EditCondition = "StackingType == EItemStackingType::Stackable", ClampMin = "1"))
	int32 MaxStackSize = 99;

	/** Free-form tags (e.g. "Item.Weapon", "Item.KeyItem") — used by URestrictedInventoryComponent
	 *  to filter what an inventory accepts, and available for anything else you want to build on top. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FGameplayTagContainer Categories;

	/** How much currency selling ONE unit of this item back to a vendor grants. -1 = can't be sold. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 SellPrice = -1;

	UFUNCTION(BlueprintPure, Category = "Item")
	bool CanBeSold() const { return SellPrice >= 0; }
};
