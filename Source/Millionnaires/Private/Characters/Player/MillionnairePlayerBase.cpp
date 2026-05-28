#include "Characters/Player/MillionnairePlayerBase.h"

#include "DayNightWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InteractionComponent.h"
#include "InventoryComponent.h"
#include "RestrictedInventoryComponent.h"
#include "DropComponent.h"
#include "NavigationSystem.h"
#include "QuestManagerComponent.h"
#include "UI/MultiInventoryWidget.h"
#include "UI/InteractionWidget.h"
#include "UI/InventoryWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/FlashlightEquipmentComponent.h"
#include "Components/Characters/CharacterStatsComponent.h"
#include "Consumable/ConsumableComponent.h"
#include "Characters/Player/PlayerControllerInterface.h"
#include "Controller/ControllerInterface.h"
#include "Component/TraversalComponent.h"

// -------------------------------------------------
//  DEBUG COMMAND
// -------------------------------------------------
#pragma region Command

static FAutoConsoleCommand CCmdDamage(
    TEXT("test.damage"),
    TEXT("Deal 20 damage to player"),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        if (!GEngine || !GEngine->GameViewport) return;

        UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GameViewport, EGetWorldErrorMode::LogAndReturnNull);
        if (!World) return;

        APlayerController* PC = World->GetFirstPlayerController();
        if (!PC) return;

        APawn* Pawn = PC->GetPawn();
        if (!Pawn) return;

        UCharacterStatsComponent* Stats = Pawn->FindComponentByClass<UCharacterStatsComponent>();
        if (!Stats) return;

        float HealthBefore = Stats->GetCurrentHealth();
        Stats->ModifyHealth(-20.0f);
        float HealthAfter = Stats->GetCurrentHealth();

        UE_LOG(LogTemp, Warning, TEXT("✓ DAMAGE APPLIED: %.1f -> %.1f (%.1f damage)"),
            HealthBefore, HealthAfter, HealthBefore - HealthAfter);
    })
);

#pragma endregion

// -------------------------------------------------
//  CONSTRUCTOR
// -------------------------------------------------

AMillionnairePlayerBase::AMillionnairePlayerBase()
{
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
    FirstPersonMesh->SetupAttachment(GetMesh());
    FirstPersonMesh->SetOnlyOwnerSee(true);
    FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
    FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
    FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
    FirstPersonCameraComponent->SetRelativeLocationAndRotation(
        FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
    FirstPersonCameraComponent->bUsePawnControlRotation = true;
    FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
    FirstPersonCameraComponent->bEnableFirstPersonScale = true;
    FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
    FirstPersonCameraComponent->FirstPersonScale = 0.6f;

    GetMesh()->SetOwnerNoSee(true);
    GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;
    GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
    GetCharacterMovement()->AirControl = 0.5f;

    GeneralInventoryComponent = CreateDefaultSubobject<URestrictedInventoryComponent>(TEXT("GeneralInventory"));
    GeneralInventoryComponent->NumSlots = 20;
    GeneralInventoryComponent->CategoryFilter.Categories = { EItemCategory::Equipment, EItemCategory::Consumable };
    GeneralInventoryComponent->CategoryFilter.bWhitelistMode = false;
    GeneralInventoryComponent->ComponentTags.Add(FName("General"));

    EquipmentInventoryComponent = CreateDefaultSubobject<URestrictedInventoryComponent>(TEXT("EquipmentInventory"));
    EquipmentInventoryComponent->NumSlots = 10;
    EquipmentInventoryComponent->CategoryFilter.Categories = { EItemCategory::Equipment };
    EquipmentInventoryComponent->CategoryFilter.bWhitelistMode = true;
    EquipmentInventoryComponent->ComponentTags.Add(FName("Equipment"));

    ConsumableInventoryComponent = CreateDefaultSubobject<URestrictedInventoryComponent>(TEXT("ConsumableInventory"));
    ConsumableInventoryComponent->NumSlots = 8;
    ConsumableInventoryComponent->CategoryFilter.Categories = { EItemCategory::Consumable };
    ConsumableInventoryComponent->CategoryFilter.bWhitelistMode = true;
    ConsumableInventoryComponent->ComponentTags.Add(FName("Consumable"));

    DropComponent        = CreateDefaultSubobject<UDropComponent>(TEXT("DropComponent"));
    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
    InteractionComponent->bShowDebugTrace = false;
    ConsumableComponent  = CreateDefaultSubobject<UConsumableComponent>(TEXT("ConsumableComponent"));
    FlashlightComponent  = CreateDefaultSubobject<UFlashlightEquipmentComponent>(TEXT("FlashlightComponent"));
    QuestManagerComponent = CreateDefaultSubobject<UQuestManagerComponent>(TEXT("QuestManagerComponent"));
    ATraversalComponent = CreateDefaultSubobject<UTraversalComponent>(TEXT("TraversalComponent"));
}

// -------------------------------------------------
//  BEGIN PLAY
// -------------------------------------------------

void AMillionnairePlayerBase::BeginPlay()
{
    Super::BeginPlay();

    InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
    if (InteractionWidget)
    {
        InteractionWidget->AddToViewport();
        InteractionWidget->SetInteractionVisible(false);
    }

    if (DayNightWidgetClass)
    {
        DayNightWidget = CreateWidget<UDayNightWidget>(GetWorld(), DayNightWidgetClass);
        if (DayNightWidget)
            DayNightWidget->AddToViewport();
    }

    if (ConsumableComponent)
        ConsumableComponent->CacheInventoryComponents();

    if (QuestTrackerWidgetClass && IsValid(QuestManagerComponent))
    {
        QuestTrackerWidget = CreateWidget<UQuestTrackerWidget>(GetWorld(), QuestTrackerWidgetClass);
        if (IsValid(QuestTrackerWidget))
        {
            QuestTrackerWidget->AddToViewport();
            QuestTrackerWidget->InitializeTracker(QuestManagerComponent);
        }
    }

    InteractionComponent->CanInteractDelegate.BindLambda([this]() -> bool
    {
        IPlayerControllerInterface* PC = Cast<IPlayerControllerInterface>(GetController());
        return PC ? PC->CanPerform(EPlayerAction::Interact) : true;
    });
}

// -------------------------------------------------
//  TICK
// -------------------------------------------------

void AMillionnairePlayerBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (InteractionWidget && InteractionComponent)
    {
        if (InteractionComponent->HasFocusedActor())
        {
            InteractionWidget->UpdateInteractionText(InteractionComponent->GetFocusedInteractionText());
            InteractionWidget->SetInteractionVisible(true);
        }
        else
        {
            InteractionWidget->SetInteractionVisible(false);
        }
    }
}

// -------------------------------------------------
//  INPUT SETUP — vide, tout est sur le Controller
// -------------------------------------------------

void AMillionnairePlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Inputs gérés par AMillionnairesPlayerController
}

// -------------------------------------------------
//  VERBES — exécution pure, pas de décision
// -------------------------------------------------
#pragma region Verbes

void AMillionnairePlayerBase::DoMove(float Right, float Forward)
{
    if (ATraversalComponent && ATraversalComponent->IsTraversing())
    {
        ATraversalComponent->SetTraversalInput(FVector2D(Right, Forward));
        return;
    }
    
    AddMovementInput(GetActorRightVector(), Right);
    AddMovementInput(GetActorForwardVector(), Forward);
}

void AMillionnairePlayerBase::DoMoveEnd(float Right, float Forward)
{
    if (ATraversalComponent&& ATraversalComponent->IsTraversing())
    {
        ATraversalComponent->SetTraversalInput(FVector2D(Right, Forward));
    }
}

void AMillionnairePlayerBase::DoAim(float Yaw, float Pitch)
{
    AddControllerYawInput(Yaw);
    AddControllerPitchInput(Pitch);
}

void AMillionnairePlayerBase::DoJumpStart()
{
    Jump();
}

void AMillionnairePlayerBase::DoJumpEnd()
{
    StopJumping();
}

void AMillionnairePlayerBase::DoInteract()
{
    if (InteractionComponent)
        InteractionComponent->Interact();
}

#pragma endregion

// -------------------------------------------------
//  INVENTAIRE / DROP
// -------------------------------------------------
#pragma region Inventory

void AMillionnairePlayerBase::OnInventoryPressed()
{
    InitializeInventoryWidget();
    if (!MultiInventoryWidget) return;

    if (bIsInventoryOpen)
    {
        MultiInventoryWidget->RemoveFromParent();
        bIsInventoryOpen = false;

        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            PC->SetInputMode(FInputModeGameOnly());
            PC->bShowMouseCursor = false;
        }
    }
    else
    {
        MultiInventoryWidget->AddToViewport();
        bIsInventoryOpen = true;

        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(MultiInventoryWidget->TakeWidget());
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
    }
}

void AMillionnairePlayerBase::SelectInventorySlot(int32 SlotIndex, UInventoryComponent* InventoryComp)
{
    if (!InventoryComp || !InventoryComp->IsValidSlotIndex(SlotIndex)) return;
    SelectedSlotIndex = SlotIndex;
    SelectedInventoryComponent = InventoryComp;
}

void AMillionnairePlayerBase::OnDropItemPressed()
{
    if (!SelectedInventoryComponent || !DropComponent) return;
    if (SelectedSlotIndex == INDEX_NONE) return;
    if (!SelectedInventoryComponent->IsValidSlotIndex(SelectedSlotIndex)) return;

    FItemSlot Slot = SelectedInventoryComponent->GetSlot(SelectedSlotIndex);
    if (Slot.IsEmpty()) return;

    bool bDropSuccess = DropComponent->DropFromInventory(SelectedInventoryComponent, SelectedSlotIndex);
    if (bDropSuccess && SelectedInventoryComponent->GetSlot(SelectedSlotIndex).IsEmpty())
    {
        SelectedSlotIndex = INDEX_NONE;
        SelectedInventoryComponent = nullptr;
    }
}

void AMillionnairePlayerBase::InitializeInventoryWidget()
{
    if (!MultiInventoryWidgetClass || MultiInventoryWidget) return;

    MultiInventoryWidget = CreateWidget<UMultiInventoryWidget>(GetWorld(), MultiInventoryWidgetClass);
    if (!MultiInventoryWidget) return;

    MultiInventoryWidget->InitializeAllInventories(
        EquipmentInventoryComponent,
        ConsumableInventoryComponent,
        GeneralInventoryComponent);

    auto BindSlot = [&](UInventoryWidget* Widget)
    {
        if (Widget)
            Widget->OnSlotSelected.AddDynamic(this, &AMillionnairePlayerBase::SelectInventorySlot);
    };

    BindSlot(MultiInventoryWidget->EquipmentInventory);
    BindSlot(MultiInventoryWidget->ConsumableInventory);
    BindSlot(MultiInventoryWidget->GeneralInventory);
}

#pragma endregion

// -------------------------------------------------
//  CONSOMMABLES
// -------------------------------------------------
#pragma region Consumables

void AMillionnairePlayerBase::OnUseHealthPressed()
{
    if (!ConsumableComponent) return;
    FGameplayTagContainer HealthTags;
    HealthTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Consumable.Health")));
    ConsumableComponent->ConsumeFirstItemWithTags(HealthTags);
}

void AMillionnairePlayerBase::OnUseFoodPressed()
{
    if (!ConsumableComponent) return;
    FGameplayTagContainer FoodTags;
    FoodTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Consumable.Food")));
    ConsumableComponent->ConsumeFirstItemWithTags(FoodTags);
}

void AMillionnairePlayerBase::OnUseBatteryPressed()
{
    if (!ConsumableComponent) return;
    FGameplayTagContainer BatteryTags;
    BatteryTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Consumable.Battery")));
    ConsumableComponent->ConsumeFirstItemWithTags(BatteryTags);
}

void AMillionnairePlayerBase::UseConsumableFromSlot(int32 SlotIndex, UInventoryComponent* InventoryComp)
{
    if (!ConsumableComponent || !InventoryComp) return;
    if (!InventoryComp->IsValidSlotIndex(SlotIndex)) return;

    FItemSlot Slot = InventoryComp->GetSlot(SlotIndex);
    if (Slot.IsEmpty()) return;

    const FItemData* ItemData = Slot.GetItemData();
    if (!ItemData || !ItemData->bIsConsumable) return;

    ConsumableComponent->ConsumeItemFromSlot(InventoryComp, SlotIndex);
}

#pragma endregion

// -------------------------------------------------
//  FLASHLIGHT
// -------------------------------------------------
#pragma region Flashlight

void AMillionnairePlayerBase::OnToggleFlashlightPressed()
{
    if (!FlashlightComponent || !FlashlightComponent->IsFlashlightEquipped()) return;
    FlashlightComponent->ToggleFlashlight();
}

void AMillionnairePlayerBase::OnEquipFlashlightPressed()
{
    if (!FlashlightComponent) return;

    if (FlashlightComponent->IsFlashlightEquipped())
    {
        FlashlightComponent->UnequipFlashlight();
        return;
    }

    if (!ConsumableComponent) return;

    for (UInventoryComponent* Inventory : ConsumableComponent->GetAllInventories())
    {
        if (!Inventory) continue;
        const TArray<FItemSlot>& Slots = Inventory->GetAllSlots();
        for (int32 i = 0; i < Slots.Num(); i++)
        {
            if (Slots[i].IsEmpty()) continue;
            const FItemData* ItemData = Slots[i].GetItemData();
            if (!ItemData || !ItemData->bIsFlashlight) continue;

            FlashlightComponent->EquipFlashlight(Slots[i].ItemHandle, ItemData->InitialBatteryCharge);
            return;
        }
    }
}

#pragma endregion

// -------------------------------------------------
//  INVENTORY INTERFACE
// -------------------------------------------------
#pragma region InventoryInterface

bool AMillionnairePlayerBase::HasSpaceForItemInAnyInventory_Implementation(
    const FDataTableRowHandle& ItemHandle, int32 Amount) const
{
    const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("CheckSpace"));
    if (!ItemData) return false;

    if (ItemData->Category == EItemCategory::Equipment && EquipmentInventoryComponent)
        if (IInventoryInterface::Execute_HasSpaceForItem(EquipmentInventoryComponent, ItemHandle, Amount))
            return true;
    else if (ItemData->Category == EItemCategory::Consumable && ConsumableInventoryComponent)
        if (IInventoryInterface::Execute_HasSpaceForItem(ConsumableInventoryComponent, ItemHandle, Amount))
            return true;

    if (GeneralInventoryComponent)
        if (IInventoryInterface::Execute_HasSpaceForItem(GeneralInventoryComponent, ItemHandle, Amount))
            return true;

    return false;
}

void AMillionnairePlayerBase::TryStartTraversal(ATraversalActor* Target)
{
    UE_LOG(LogTemp, Warning, TEXT("TraversalComponent: %p"), ATraversalComponent);
    UE_LOG(LogTemp, Warning, TEXT("TraversalComponent2: %p"), ATraversalComponent);
    if (!ATraversalComponent) return;

    ATraversalComponent->StartTraversal(Target);
    
 /*   if (IControllerInterface* Cam =
      Cast<IControllerInterface>(GetController()))
    {
        Cam->SetPlayerMode(EPlayerMode::Ladder, Ladder);
        
    }
*/
}


int32 AMillionnairePlayerBase::AddItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount)
{
    if (ItemHandle.IsNull() || Amount <= 0) return Amount;

    const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("AddItem"));
    if (!ItemData) return Amount;

    int32 Remaining = Amount;
    auto AddToInventory = [&](UInventoryComponent* InvComp)
    {
        Remaining = IInventoryInterface::Execute_AddItem(InvComp, ItemHandle, Remaining);
    };

    if (ItemData->Category == EItemCategory::Equipment && EquipmentInventoryComponent)
        AddToInventory(EquipmentInventoryComponent);
    else if (ItemData->Category == EItemCategory::Consumable && ConsumableInventoryComponent)
        AddToInventory(ConsumableInventoryComponent);

    if (Remaining > 0 && GeneralInventoryComponent)
        AddToInventory(GeneralInventoryComponent);

    return Remaining;
}

bool AMillionnairePlayerBase::HasSpaceForItem_Implementation(
    const FDataTableRowHandle& ItemHandle, int32 Amount) const
{
    if (ItemHandle.IsNull() || Amount <= 0) return false;
    const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("HasSpaceForItem"));
    if (!ItemData) return false;

    return TryInventoryOperation(ItemData, [&](UInventoryComponent* InvComp)
    {
        return IInventoryInterface::Execute_HasSpaceForItem(InvComp, ItemHandle, Amount);
    });
}

#pragma endregion