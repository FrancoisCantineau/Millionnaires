#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Consumable/ConsumableInterface.h"
#include "Engine/DataTable.h"

#include "ConsumableComponent.generated.h"

struct FConsumableContext;
struct FItemData;
class UInventoryComponent;
class URestrictedInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemConsumed,FDataTableRowHandle, ItemHandle, EConsumableResult, Result, int32, SlotIndex);

/**
 * Component that handles consuming items from an actor's inventory
 */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UConsumableComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UConsumableComponent();

protected:

    virtual void BeginPlay() override;

public:

    /** Event fired when an item is consumed */
    UPROPERTY(BlueprintAssignable, Category = "Consumable")
    FOnItemConsumed OnItemConsumed;

    /** If true, log consumption attempts for debugging */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bDebugMode = false;

    /** Cache references to all inventory components on the owner */
    void CacheInventoryComponents();

    /** Get all inventories from the cached list */
    TArray<UInventoryComponent*> GetAllInventories() const;

#pragma region Consumption Methods

    /** Consume an item from a specific inventory slot */
    UFUNCTION(BlueprintCallable, Category = "Consumable")
    EConsumableResult ConsumeItemFromSlot(UInventoryComponent* InventoryComp, 
        int32 SlotIndex, AActor* Target = nullptr);

    /** Consume the first item found with matching gameplay tags */
    UFUNCTION(BlueprintCallable, Category = "Consumable")
    EConsumableResult ConsumeFirstItemWithTags(const FGameplayTagContainer& RequiredTags, 
        AActor* Target = nullptr);

    /** Check if an item can be consumed */
    UFUNCTION(BlueprintCallable, Category = "Consumable")
    bool CanConsumeItem(const FDataTableRowHandle& ItemHandle, AActor* Target = nullptr) const;

#pragma endregion

#pragma region Query Methods

    /** Find all inventory slots containing consumable items with matching tags */
    UFUNCTION(BlueprintCallable, Category = "Consumable")
    TArray<int32> FindConsumableSlotsWithTags(const FGameplayTagContainer& RequiredTags) const;

    /** Check if the owner has any consumable with the specified tags */
    UFUNCTION(BlueprintPure, Category = "Consumable")
    bool HasConsumableWithTags(const FGameplayTagContainer& RequiredTags) const;

#pragma endregion

protected:

#pragma region Internal Helpers
    
    /** Execute consumption of an item */
    EConsumableResult ExecuteConsumption(const FDataTableRowHandle& ItemHandle, 
        UInventoryComponent* InventoryComp, int32 SlotIndex, AActor* Target);

    /** Apply all effects from an item */
    EConsumableResult ApplyConsumableEffects(const FItemData* ItemData, 
        const FConsumableContext& Context);

#pragma endregion
    
    /** Cached inventory components */
    UPROPERTY()
    TArray<TObjectPtr<UInventoryComponent>> CachedInventories;
};