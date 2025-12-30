#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"

#include "ConsumableInterface.generated.h"

/**
 * Result of a consumption attempt
 */
UENUM(BlueprintType)
enum class EConsumableResult : uint8
{
    Success,
    Failed_NoTarget,
    Failed_InvalidItem,
    Failed_CannotConsume,
    Failed_AlreadyFull
};

/**
 * Context passed to consumable effects
 */
USTRUCT(BlueprintType)
struct GAMEINTERFACES_API FConsumableContext
{
    GENERATED_BODY()

    /** Actor initiating the consumption (usually the player) */
    UPROPERTY(BlueprintReadWrite, Category = "Consumable")
    AActor* Instigator = nullptr;

    /** Primary target of the consumption (could be the instigator or another actor) */
    UPROPERTY(BlueprintReadWrite, Category = "Consumable")
    AActor* Target = nullptr;

    /** Amount being consumed (for partial consumption) */
    UPROPERTY(BlueprintReadWrite, Category = "Consumable")
    int32 Amount = 1;

    /** Optional world location for effects */
    UPROPERTY(BlueprintReadWrite, Category = "Consumable")
    FVector Location = FVector::ZeroVector;

    FConsumableContext() = default;

    FConsumableContext(AActor* InInstigator, AActor* InTarget = nullptr, int32 InAmount = 1)
        : Instigator(InInstigator)
        , Target(InTarget ? InTarget : InInstigator)
        , Amount(InAmount)
    {
        if (Target)
        {
            Location = Target->GetActorLocation();
        }
    }
};

UINTERFACE(MinimalAPI, Blueprintable)
class UConsumableInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for consumable items that can be used/consumed
 */
class GAMEINTERFACES_API IConsumableInterface
{
    GENERATED_BODY()

public:

    /** Check if this consumable can be used in the current context */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Consumable")
    bool CanConsume(const FConsumableContext& Context) const;

    /** Use the item and apply its effects */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Consumable")
    EConsumableResult Consume(const FConsumableContext& Context);

    /** Get the gameplay tags that identify this consumable's purpose */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Consumable")
    FGameplayTagContainer GetConsumableTags() const;
    
};