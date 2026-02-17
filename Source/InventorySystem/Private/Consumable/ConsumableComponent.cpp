#include "Consumable/ConsumableComponent.h"
#include "InventoryComponent.h"
#include "ItemData.h"
#include "ItemSlot.h"

UConsumableComponent::UConsumableComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UConsumableComponent::BeginPlay()
{
    Super::BeginPlay();
    CacheInventoryComponents();
}

#pragma region Consumption Methods

/*
 * 
 */
EConsumableResult UConsumableComponent::ConsumeItemFromSlot(
    UInventoryComponent* InventoryComp, int32 SlotIndex, AActor* Target)
{
    if (!InventoryComp)
    {
        return EConsumableResult::Failed_NoTarget;
    }

    if (!InventoryComp->IsValidSlotIndex(SlotIndex))
    {
        return EConsumableResult::Failed_InvalidItem;
    }

    FItemSlot Slot = InventoryComp->GetSlot(SlotIndex);
    if (Slot.IsEmpty())
    {
        return EConsumableResult::Failed_InvalidItem;
    }

    return ExecuteConsumption(Slot.ItemHandle, InventoryComp, SlotIndex, Target);
}

/*
 * 
 */
EConsumableResult UConsumableComponent::ConsumeFirstItemWithTags(
    const FGameplayTagContainer& RequiredTags, AActor* Target)
{
    if (RequiredTags.IsEmpty())
    {
        return EConsumableResult::Failed_InvalidItem;
    }

    for (UInventoryComponent* Inventory : GetAllInventories())
    {
        if (!Inventory)
        {
            continue;
        }

        const TArray<FItemSlot>& Slots = Inventory->GetAllSlots();
        
        for (int32 i = 0; i < Slots.Num(); ++i)
        {
            const FItemSlot& Slot = Slots[i];
            
            if (Slot.IsEmpty())
            {
                continue;
            }

            const FItemData* ItemData = Slot.GetItemData();
            if (!ItemData || !ItemData->bIsConsumable)
            {
                continue;
            }

            FGameplayTagContainer EffectTags = ItemData->GetConsumableTags();
            const FGameplayTagContainer& ItemTags = ItemData->ItemTags;
            
            if (EffectTags.HasAny(RequiredTags) || ItemTags.HasAny(RequiredTags))
            {
                return ConsumeItemFromSlot(Inventory, i, Target);
            }
        }
    }
    return EConsumableResult::Failed_InvalidItem;
}

/*
 * 
 */
bool UConsumableComponent::CanConsumeItem(const FDataTableRowHandle& ItemHandle, AActor* Target) const
{
    if (ItemHandle.IsNull())
    {
        return false;
    }

    const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("CanConsumeItem"));
    if (!ItemData || !ItemData->bIsConsumable)
    {
        return false;
    }

    if (!ItemData->ConsumableConfig)
    {
        return false;
    }

    AActor* TargetActor = Target ? Target : GetOwner();
    if (!TargetActor)
    {
        return false;
    }

    FConsumableContext Context(GetOwner(), TargetActor);

    for (const UConsumableEffect* Effect : ItemData->ConsumableConfig->Effects)
    {
        if (Effect && Effect->CanApplyEffect(Context))
        {
            return true;
        }
    }

    return false;
}

#pragma endregion

#pragma region Query Methods

/*
 * 
 */
TArray<int32> UConsumableComponent::FindConsumableSlotsWithTags(
    const FGameplayTagContainer& RequiredTags) const
{
    TArray<int32> MatchingSlots;

    if (RequiredTags.IsEmpty())
    {
        return MatchingSlots;
    }

    for (UInventoryComponent* Inventory : GetAllInventories())
    {
        if (!Inventory)
        {
            continue;
        }

        const TArray<FItemSlot>& Slots = Inventory->GetAllSlots();
        
        for (int32 i = 0; i < Slots.Num(); ++i)
        {
            const FItemSlot& Slot = Slots[i];
            
            if (Slot.IsEmpty())
            {
                continue;
            }

            const FItemData* ItemData = Slot.GetItemData();
            if (!ItemData || !ItemData->bIsConsumable)
            {
                continue;
            }

            FGameplayTagContainer EffectTags = ItemData->GetConsumableTags();
            const FGameplayTagContainer& ItemTags = ItemData->ItemTags;
            
            if (EffectTags.HasAny(RequiredTags) || ItemTags.HasAny(RequiredTags))
            {
                MatchingSlots.Add(i);
            }
        }
    }

    return MatchingSlots;
}

/*
 * 
 */
bool UConsumableComponent::HasConsumableWithTags(const FGameplayTagContainer& RequiredTags) const
{
    return FindConsumableSlotsWithTags(RequiredTags).Num() > 0;
}

#pragma endregion

#pragma region Internal Helpers

/*
 * 
 */
void UConsumableComponent::CacheInventoryComponents()
{
    CachedInventories.Empty();

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    TArray<UInventoryComponent*> FoundInventories;
    Owner->GetComponents<UInventoryComponent>(FoundInventories);

    for (UInventoryComponent* Inventory : FoundInventories)
    {
        if (Inventory)
        {
            CachedInventories.Add(Inventory);
        }
    }
}

/*
 * 
 */
TArray<UInventoryComponent*> UConsumableComponent::GetAllInventories() const
{
    TArray<UInventoryComponent*> Result;
    
    for (const TObjectPtr<UInventoryComponent>& Inventory : CachedInventories)
    {
        if (Inventory)
        {
            Result.Add(Inventory.Get());
        }
    }
    
    return Result;
}

/*
 * 
 */
EConsumableResult UConsumableComponent::ExecuteConsumption(
    const FDataTableRowHandle& ItemHandle, 
    UInventoryComponent* InventoryComp, 
    int32 SlotIndex, 
    AActor* Target)
{
    
    if (ItemHandle.IsNull())
    {
        return EConsumableResult::Failed_InvalidItem;
    }

    const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("ExecuteConsumption"));
    if (!ItemData)
    {
        return EConsumableResult::Failed_InvalidItem;
    }
    
    if (!ItemData->bIsConsumable)
    {
        return EConsumableResult::Failed_CannotConsume;
    }

    AActor* TargetActor = Target ? Target : GetOwner();
    FConsumableContext Context(GetOwner(), TargetActor);
    
    EConsumableResult Result = ApplyConsumableEffects(ItemData, Context);
    
    if (Result == EConsumableResult::Success && ItemData->bConsumeOnUse)
    {
        InventoryComp->RemoveOneFromStack(SlotIndex);
    }

    OnItemConsumed.Broadcast(ItemHandle, Result, SlotIndex);

    return Result;
}

/*
 * 
 */
EConsumableResult UConsumableComponent::ApplyConsumableEffects(
    const FItemData* ItemData, 
    const FConsumableContext& Context)
{
    if (!ItemData || !ItemData->ConsumableConfig)
    {
        return EConsumableResult::Failed_InvalidItem;
    }

    const TArray<UConsumableEffect*>& Effects = ItemData->ConsumableConfig->Effects;
    
    if (Effects.IsEmpty())
    {
        return EConsumableResult::Failed_InvalidItem;
    }
    
    bool bAnyEffectApplied = false;
    EConsumableResult LastResult = EConsumableResult::Failed_CannotConsume;

    for (UConsumableEffect* Effect : Effects)
    {
        if (!Effect)
        {
            continue;
        }
        
        if (!Effect->CanApplyEffect(Context))
        {
            LastResult = EConsumableResult::Failed_AlreadyFull;
            continue;
        }

        EConsumableResult Result = Effect->ApplyEffect(Context);
        
        if (Result == EConsumableResult::Success)
        {
            bAnyEffectApplied = true;
        }
        else
        {
            LastResult = Result;
        }
    }

    return bAnyEffectApplied ? EConsumableResult::Success : LastResult;
}

#pragma endregion