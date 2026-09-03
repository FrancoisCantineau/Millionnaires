// RestrictedInventoryComponent.h
#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "GameplayTagContainer.h"
#include "RestrictedInventoryComponent.generated.h"

/**
 * An inventory that only accepts items matching a category filter (via UItemDefinition::Categories)
 * — e.g. a hotbar that only takes "Item.Weapon", or a key-item pouch that only takes "Item.KeyItem".
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API URestrictedInventoryComponent : public UInventoryComponent
{
	GENERATED_BODY()

public:
	/** The categories this inventory cares about. Empty = no restriction (accepts everything). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Restrictions")
	FGameplayTagContainer Categories;

	/** If true, an item must have at least one tag in Categories to be allowed (whitelist).
	 *  If false, an item is rejected if it has ANY tag in Categories (blacklist). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Restrictions")
	bool bWhitelistMode = true;

	UFUNCTION(BlueprintPure, Category = "Inventory|Restrictions")
	bool CanAcceptItem(const UItemDefinition* Item) const;

protected:
	virtual bool IsItemAllowed(const UItemDefinition* Item) const override;
};
