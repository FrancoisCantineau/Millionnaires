#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Interfaces/InputReceiverInterface.h"
#include "MillionnairesCharacter.h"
#include "Logging/LogMacros.h"
#include "ItemData.h"
#include "InventoryInterface.h"
#include "TraversalInterface.h"
#include "Component/TraversalComponent.h"
#include "MillionnairePlayerBase.generated.h"

class UConsumableComponent;
class UFlashlightEquipmentComponent;
class UInventoryComponent;
class URestrictedInventoryComponent;
class UDropComponent;
class UInteractionComponent;
class UMultiInventoryWidget;
class UInteractionWidget;
class UInputComponent;

UCLASS()
class MILLIONNAIRES_API AMillionnairePlayerBase : public AMillionnairesCharacter, public IInventoryInterface, public ITraversalInterface, public IInputReceiverInterface
{
    GENERATED_BODY()

protected:

    void InitializeInventoryWidget();

#pragma region Inventory Operation Helper

    template<typename Func>
    bool TryInventoryOperation(const FItemData* ItemData, Func Operation) const
    {
        if (!ItemData) return false;

        if (ItemData->Category == EItemCategory::Equipment && EquipmentInventoryComponent)
            if (Operation(EquipmentInventoryComponent)) return true;
        else if (ItemData->Category == EItemCategory::Consumable && ConsumableInventoryComponent)
            if (Operation(ConsumableInventoryComponent)) return true;

        if (GeneralInventoryComponent)
            if (Operation(GeneralInventoryComponent)) return true;

        return false;
    }

#pragma endregion

#pragma region Inventory

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    URestrictedInventoryComponent* GeneralInventoryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    URestrictedInventoryComponent* EquipmentInventoryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    URestrictedInventoryComponent* ConsumableInventoryComponent;

#pragma endregion

#pragma region Interaction

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    UInteractionComponent* InteractionComponent;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UInteractionWidget> InteractionWidgetClass;

    UPROPERTY()
    UInteractionWidget* InteractionWidget;

#pragma endregion

#pragma region Consumable


#pragma endregion

#pragma region Flashlight

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
    UFlashlightEquipmentComponent* FlashlightComponent;

#pragma endregion

#pragma region Quests
/*
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
    UQuestManagerComponent* QuestManagerComponent;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UQuestTrackerWidget> QuestTrackerWidgetClass;

    UPROPERTY()
    UQuestTrackerWidget* QuestTrackerWidget;*/

#pragma endregion

#pragma region Day/Night

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UDayNightWidget> DayNightWidgetClass;

    UPROPERTY()
    class UDayNightWidget* DayNightWidget;

#pragma endregion

#pragma region UI
    

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    bool bIsInventoryOpen = false;

#pragma endregion

#pragma region Selection State

    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    int32 SelectedSlotIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    UInventoryComponent* SelectedInventoryComponent = nullptr;

#pragma endregion

    UFUNCTION()
    void SelectInventorySlot(int32 SlotIndex, UInventoryComponent* InventoryComp);

    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

#pragma region Interface Implementations

    virtual int32 AddItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount) override;
    virtual bool HasSpaceForItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount) const override;
    virtual bool HasSpaceForItemInAnyInventory_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount) const override;

#pragma endregion

#pragma region Traversal

    virtual void TryStartTraversal(ATraversalActor* Target) override;

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    UTraversalComponent* ATraversalComponent;

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    TObjectPtr<UContextDataAsset> DefaultContextData;
    
#pragma endregion

public:

    AMillionnairePlayerBase();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // -------------------------------------------------
    //  VERBES — appelés par le Controller
    // -------------------------------------------------

    virtual void HandleInput_Implementation(FGameplayTag Tag, const FInputActionValue& Value, ETriggerEvent TriggerEvent) override;
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoMove(float Right, float Forward);

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoMoveEnd(float Right, float Forward);
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoAim(float Yaw, float Pitch);

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoJumpStart();

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoJumpEnd();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void DoInteract();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void OnInventoryPressed();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void OnDropItemPressed();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void OnUseHealthPressed();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void OnUseFoodPressed();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void OnUseBatteryPressed();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void OnToggleFlashlightPressed();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void OnEquipFlashlightPressed();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void UseConsumableFromSlot(int32 SlotIndex, UInventoryComponent* InventoryComp);

    // -------------------------------------------------
    //  GETTERS
    // -------------------------------------------------

#pragma region Getters
    
    FORCEINLINE URestrictedInventoryComponent* GetGeneralInventory() const { return GeneralInventoryComponent; }
    FORCEINLINE URestrictedInventoryComponent* GetEquipmentInventory() const { return EquipmentInventoryComponent; }
    FORCEINLINE URestrictedInventoryComponent* GetConsumableInventory() const { return ConsumableInventoryComponent; }
    FORCEINLINE int32 GetSelectedSlotIndex() const { return SelectedSlotIndex; }
    FORCEINLINE UInventoryComponent* GetSelectedInventoryComponent() const { return SelectedInventoryComponent; }
    FORCEINLINE UFlashlightEquipmentComponent* GetFlashlightComponent() const { return FlashlightComponent; }
   // FORCEINLINE UQuestManagerComponent* GetQuestManager() const { return QuestManagerComponent; }

#pragma endregion





    
};