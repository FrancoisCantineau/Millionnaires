#include "Characters/Player/MillionnairePlayerBase.h"

#include "ActionComponent.h"
#include "DayNightWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InteractionComponent.h"
#include "InventoryComponent.h"
#include "RestrictedInventoryComponent.h"
#include "InputActionValue.h"
#include "NavigationSystem.h"
#include "UI/InteractionWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/FlashlightEquipmentComponent.h"
#include "Components/Characters/CharacterStatsComponent.h"
#include "Component/TraversalComponent.h"
#include "System/Tags/MillionnaireGameplayTags_Actions.h"

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

        UE_LOG(LogTemp, Warning, TEXT("DAMAGE APPLIED: %.1f -> %.1f (%.1f damage)"),
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
    GeneralInventoryComponent->ComponentTags.Add(FName("General"));

    EquipmentInventoryComponent = CreateDefaultSubobject<URestrictedInventoryComponent>(TEXT("EquipmentInventory"));
    EquipmentInventoryComponent->NumSlots = 10;
    EquipmentInventoryComponent->ComponentTags.Add(FName("Equipment"));

    ConsumableInventoryComponent = CreateDefaultSubobject<URestrictedInventoryComponent>(TEXT("ConsumableInventory"));
    ConsumableInventoryComponent->NumSlots = 8;
    ConsumableInventoryComponent->ComponentTags.Add(FName("Consumable"));
    
    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
    InteractionComponent->bShowDebugTrace = false;
    FlashlightComponent  = CreateDefaultSubobject<UFlashlightEquipmentComponent>(TEXT("FlashlightComponent"));
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
    

   /* if (QuestTrackerWidgetClass && IsValid(QuestManagerComponent))
    {
        QuestTrackerWidget = CreateWidget<UQuestTrackerWidget>(GetWorld(), QuestTrackerWidgetClass);
        if (IsValid(QuestTrackerWidget))
        {
            QuestTrackerWidget->AddToViewport();
            QuestTrackerWidget->AddToViewport();
            QuestTrackerWidget->InitializeTracker(QuestManagerComponent);
        }
    }*/

    // CLEANUP: was routing through IControllerInterface::Execute_CanPerform(PC, EPlayerAction::Interact) -
    // a round-trip through the Controller for something ActionComponent (already on this Pawn,
    // already consulted the same way by DoJumpStart below) answers directly. Same pattern, no
    // detour through the Controller needed.
    InteractionComponent->CanInteractDelegate.BindLambda([this]() -> bool
    {
        return ActionComponent && ActionComponent->CanPerform(TAG_Action_Interact);
    });

    FActiveContext DefaultContext;
    DefaultContext.Definition = DefaultContextData;
    DefaultContext.InputReceiver = this;
    ContextComponent->AddContext(DefaultContext);
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

void AMillionnairePlayerBase::HandleInput_Implementation(FGameplayTag Tag, const FInputActionValue& Value, ETriggerEvent TriggerEvent)
{
    if (Tag ==  (TAG_Action_Jump))
    {
        Jump();
        return;
    }

    if (Tag == (TAG_Action_Interact))
    {
        DoInteract();
        return;
    }
    if (Tag == (TAG_Action_Move))
    {
        FVector2D MoveVector = Value.Get<FVector2D>();
        DoMove(MoveVector.X, MoveVector.Y);
    }
}

// -------------------------------------------------
//  INPUT SETUP - vide, tout est sur le Controller
// -------------------------------------------------

void AMillionnairePlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Inputs gérés par AMillionnairesPlayerController
}

// -------------------------------------------------
//  VERBES - exécution pure, pas de décision
// -------------------------------------------------
#pragma region Verbes

void AMillionnairePlayerBase::DoMove(float Right, float Forward)
{
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
    if (ActionComponent->CanPerform(TAG_Action_Jump))
    {
        Jump();
    }
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

    
}

void AMillionnairePlayerBase::SelectInventorySlot(int32 SlotIndex, UInventoryComponent* InventoryComp)
{
    if (!InventoryComp || !InventoryComp->IsValidSlotIndex(SlotIndex)) return;
    SelectedSlotIndex = SlotIndex;
    SelectedInventoryComponent = InventoryComp;
}

void AMillionnairePlayerBase::OnDropItemPressed()
{
   
}

void AMillionnairePlayerBase::InitializeInventoryWidget()
{
   
}

#pragma endregion

// -------------------------------------------------
//  CONSOMMABLES
// -------------------------------------------------
#pragma region Consumables

void AMillionnairePlayerBase::OnUseHealthPressed()
{
   
}

void AMillionnairePlayerBase::OnUseFoodPressed()
{
   
}

void AMillionnairePlayerBase::OnUseBatteryPressed()
{
   
}

void AMillionnairePlayerBase::UseConsumableFromSlot(int32 SlotIndex, UInventoryComponent* InventoryComp)
{
   
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
    if (!ATraversalComponent) return;

    ATraversalComponent->StartTraversal(Target);
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
