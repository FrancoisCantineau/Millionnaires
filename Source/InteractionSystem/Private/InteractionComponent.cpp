#include "InteractionComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "ItemActor.h"
#include "InventoryComponent.h"

#pragma region Lifecycle

UInteractionComponent::UInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    FocusedActor = nullptr;
    CheckTimer = 0.0f;
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    CheckTimer += DeltaTime;

    if (CheckTimer >= CheckInterval)
    {
       CheckForInteractables();
       CheckTimer = 0.0f;
    }
}

#pragma endregion

#pragma region Internal Helpers

/**
 * Detect interactable actors in front of player with sphere trace
 */
void UInteractionComponent::CheckForInteractables()
{
    FVector CameraLocation;
    FVector CameraDirection;

    if (!GetCameraViewPoint(CameraLocation, CameraDirection))
    {
       FocusedActor = nullptr;
       return;
    }
   
    const FVector TraceEnd = CameraLocation + (CameraDirection * InteractionDistance);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());

   // Perform sphere trace
    FHitResult HitResult;
    const bool bHit = GetWorld()->SweepSingleByChannel(
       HitResult,
       CameraLocation,
       TraceEnd,
       FQuat::Identity,
       ECC_Visibility,
       FCollisionShape::MakeSphere(InteractionRadius),
       QueryParams
    );

   // Debug visualization
    if (bShowDebugTrace)
    {
       DrawDebugLine(
          GetWorld(),
          CameraLocation,
          TraceEnd,
          bHit ? FColor::Green : FColor::Red,
          false,
          CheckInterval,
          0,
          2.0f
       );

       // Draw sphere at hit location
       if (bHit)
       {
          DrawDebugSphere(
             GetWorld(),
             HitResult.Location,
             InteractionRadius,
             12,
             FColor::Green,
             false,
             CheckInterval
          );
       }
    }

    // Process hit result
    if (bHit && HitResult.GetActor())
    {
       AActor* HitActor = HitResult.GetActor();

       if (HitActor->Implements<UInteractionInterface>())
       {
          if (FocusedActor != HitActor)
          {
             FocusedActor = HitActor;
          }
          return;
       }
    }

    // No valid interactable found in range
    if (FocusedActor != nullptr)
    {
       FocusedActor = nullptr;
    }
}

/**
 * Get player camera information for interaction traces
 */
bool UInteractionComponent::GetCameraViewPoint(FVector& OutLocation, FVector& OutDirection) const
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
       return false;
    }

    // Try to find camera component
    UCameraComponent* CameraComponent = Character->FindComponentByClass<UCameraComponent>();
    if (CameraComponent)
    {
       OutLocation = CameraComponent->GetComponentLocation();
       OutDirection = CameraComponent->GetForwardVector();
       return true;
    }

    // Fallback to actor eyes viewpoint
    FRotator ViewRotation;
    Character->GetActorEyesViewPoint(OutLocation, ViewRotation);
    OutDirection = ViewRotation.Vector();
    return true;
}

#pragma endregion

#pragma region Public Interface

/**
 * Trigger interaction with focused actor
 */
void UInteractionComponent::Interact()
{
    if (!FocusedActor)
    {
       UE_LOG(LogTemp, Warning, TEXT("No focused actor to interact with."));
       return;
    }
   
    if (!FocusedActor->Implements<UInteractionInterface>())
    {
       UE_LOG(LogTemp, Warning, TEXT(" Focused actor does not implement InteractionInterface."));
       return;
    }
   
    IInteractionInterface::Execute_Interact(FocusedActor, GetOwner());
}

/**
 * Get interaction prompt text for UI
 */
FText UInteractionComponent::GetFocusedInteractionText() const
{
   if (!FocusedActor)
   {
      return FText::GetEmpty();
   }
    
   if (!FocusedActor->Implements<UInteractionInterface>())
   {
      return FText::GetEmpty();
   }
    
   // Special case: if focused actor is an item, check inventory space
   AItemActor* ItemActor = Cast<AItemActor>(FocusedActor);
   if (ItemActor && !ItemActor->ItemHandle.IsNull())
   {
      IInventoryInterface* InventoryInterface = Cast<IInventoryInterface>(GetOwner());
       
      if (InventoryInterface)
      {
         const bool bHasSpace = IInventoryInterface::Execute_HasSpaceForItemInAnyInventory(
             GetOwner(),
             ItemActor->ItemHandle,
             ItemActor->StackAmount
         );
          
         if (!bHasSpace)
         {
            return FText::FromString("Inventory Full!");
         }
      }
   }
   
   return IInteractionInterface::Execute_GetInteractionDisplayName(FocusedActor);
}

#pragma endregion