#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"

#include "ItemData.generated.h"

#pragma region Forward Declarations

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
    None = 0,
    Equipment,
    Consumable,
    Resource,
    QuestItem,
    Weapon,
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
    None = 0,
    Weapon,
    Helmet,
    Chest,
    Gloves,
    Boots
};

#pragma endregion

#pragma region ItemData 

/**
 * Base struct for item data in DataTable
 */
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    TSoftObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    UStaticMesh* WorldMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    EItemCategory Category = EItemCategory::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    bool bIsStackable = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", 
        meta = (EditCondition = "bIsStackable", ClampMin = "1"))
    int32 MaxStackSize = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", 
        meta = (EditCondition = "Category == EItemCategory::Equipment"))
    EEquipmentType EquipmentType = EEquipmentType::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FGameplayTagContainer ItemTags;

    FItemData()
        : DisplayName(FText::FromString("Item"))
        , Description(FText::FromString("An item"))
        , Icon(nullptr)
        , WorldMesh(nullptr)
    {}

    FORCEINLINE bool IsEquipment() const 
    { 
        return Category == EItemCategory::Equipment; 
    }
    
    FORCEINLINE bool IsConsumable() const 
    { 
        return Category == EItemCategory::Consumable; 
    }
    
    FORCEINLINE bool IsResource() const 
    { 
        return Category == EItemCategory::Resource; 
    }
    
    FORCEINLINE bool IsQuestItem() const 
    { 
        return Category == EItemCategory::QuestItem; 
    }
    
    FORCEINLINE int32 GetMaxStackSize() const 
    { 
        return bIsStackable ? MaxStackSize : 1; 
    }

    FORCEINLINE bool HasValidIcon() const
    {
        return !Icon.IsNull();
    }

    FORCEINLINE bool HasValidMesh() const
    {
        return WorldMesh != nullptr;
    }

    FORCEINLINE bool HasTag(const FGameplayTag& Tag) const
    {
        return ItemTags.HasTag(Tag);
    }

    FORCEINLINE bool HasAnyTag(const FGameplayTagContainer& Tags) const
    {
        return ItemTags.HasAny(Tags);
    }

    FORCEINLINE bool HasAllTags(const FGameplayTagContainer& Tags) const
    {
        return ItemTags.HasAll(Tags);
    }
};

#pragma endregion

#pragma region ItemData Library

/**
 * Blueprint function library for ItemData operations
 */
UCLASS()
class GAMEINTERFACES_API UItemDataLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    
    UFUNCTION(BlueprintPure, Category = "ItemData")
    static bool IsItemEquipment(const FItemData& ItemData)
    {
        return ItemData.IsEquipment();
    }

    UFUNCTION(BlueprintPure, Category = "ItemData")
    static bool IsItemConsumable(const FItemData& ItemData)
    {
        return ItemData.IsConsumable();
    }

    UFUNCTION(BlueprintPure, Category = "ItemData")
    static bool IsItemStackable(const FItemData& ItemData)
    {
        return ItemData.bIsStackable;
    }

    UFUNCTION(BlueprintPure, Category = "ItemData")
    static int32 GetItemMaxStackSize(const FItemData& ItemData)
    {
        return ItemData.GetMaxStackSize();
    }

    UFUNCTION(BlueprintPure, Category = "ItemData")
    static FString GetItemCategoryString(const FItemData& ItemData)
    {
        switch (ItemData.Category)
        {
            case EItemCategory::Equipment: return TEXT("Equipment");
            case EItemCategory::Consumable: return TEXT("Consumable");
            case EItemCategory::Resource: return TEXT("Resource");
            case EItemCategory::QuestItem: return TEXT("Quest Item");
            default: return TEXT("None");
        }
    }

    UFUNCTION(BlueprintPure, Category = "ItemData")
    static FString GetEquipmentTypeString(const FItemData& ItemData)
    {
        if (!ItemData.IsEquipment())
        {
            return TEXT("Not Equipment");
        }

        switch (ItemData.EquipmentType)
        {
            case EEquipmentType::Weapon: return TEXT("Weapon");
            case EEquipmentType::Helmet: return TEXT("Helmet");
            case EEquipmentType::Chest: return TEXT("Chest");
            case EEquipmentType::Gloves: return TEXT("Gloves");
            case EEquipmentType::Boots: return TEXT("Boots");
            default: return TEXT("None");
        }
    }
};

#pragma endregion