#include "Component/TraversalComponent.h"

#include "TraversalActor.h"
#include "ContextComponent.h"
#include "Data/ContextStructData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/ArrowComponent.h"
#include "Character/CharacterInterface.h"

// ============================================================
// INIT
// ============================================================

UTraversalComponent::UTraversalComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UTraversalComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerCharacter    = Cast<ACharacter>(GetOwner());
    ContextComponent  = GetOwner()->FindComponentByClass<UContextComponent>();
}

// ============================================================
// START
// ============================================================

void UTraversalComponent::StartTraversal(ATraversalActor* Target)
{
    if (!Target || !OwnerCharacter) return;

    CurrentTarget = Target;
    EntryPoint    = Target->GetEntryPointForCharacter(OwnerCharacter);

    // Freeze character
    SetMovementEnabled(false);
    OwnerCharacter->bUseControllerRotationYaw          = false;
    OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
    OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    State = ETraversalState::Approaching;

    // Push context — la transition (position + rotation) est gérée par ContextTransitionComponent
    if (ContextComponent)
    {
        FActiveContext Context;
        Context.Definition    = Target->GetContextData();
        Context.Source        = Target;
        Context.SnapTarget    = Target->GetEntryPointForCharacter(OwnerCharacter);       
        Context.InputReceiver = this;

        // On passe la EntryRotation directement dans le contexte pour que
        // ContextTransitionComponent utilise la bonne orientation
       // Context.EntryRotation = Target->GetEntryRotation(OwnerCharacter);

        ContextComponent->AddContext(Context);
        OnTransitionFinished();
       
        // → ContextTransitionComponent::HandleContextAdded sera appelé
        // → il gère le blend position/rotation puis appelle OnTransitionFinished()
    }
}

// ============================================================
// CALLBACK : ContextTransitionComponent a fini sa transition
// ============================================================

void UTraversalComponent::OnTransitionFinished()
{

    State = ETraversalState::Traversing;
    
    if (OwnerCharacter)
    {
        OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
    }
}

// ============================================================
// EXIT
// ============================================================

void UTraversalComponent::ExitTraversal()
{
    if (!OwnerCharacter) return;
    
    
    CurrentTarget = nullptr;
    State         = ETraversalState::None;

    OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OwnerCharacter->bUseControllerRotationYaw = true;
    OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
    OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

   
}

void UTraversalComponent::StopTraversal()
{
    ExitTraversal();
}

// ============================================================
// MOVEMENT HELPER
// ============================================================

void UTraversalComponent::SetMovementEnabled(bool bEnabled)
{
    if (!OwnerCharacter) return;

    if (bEnabled)
        OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    else
        OwnerCharacter->GetCharacterMovement()->DisableMovement();
}

// ============================================================
// INPUT
// ============================================================

void UTraversalComponent::HandleInput_Implementation(FGameplayTag Tag, const FInputActionValue& Value)
{
    FVector2D MoveVector = Value.Get<FVector2D>();
    SetTraversalInput(MoveVector);
}

void UTraversalComponent::SetTraversalInput(FVector2D Input)
{
    if (!CurrentTarget) return;

    CurrentTarget->HandleTraversalInput(this, Input);
}

void UTraversalComponent::OnTraversalNotify(ETraversalNotifyType EventType)
{
    if (!CurrentTarget) return;

    CurrentTarget->HandleTraversalNotify(EventType, this);
}

// ============================================================
// TRACE
// ============================================================

bool UTraversalComponent::TraceInDirection(const FVector& Direction, float Distance)
{
    if (!OwnerCharacter) return false;

    FVector Start = OwnerCharacter->GetCapsuleComponent()->GetComponentLocation();
    FVector End   = Start + Direction.GetSafeNormal() * Distance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit, Start, End,
        ECC_Visibility,
        Params
    );

    if (bDrawDebug)
    {
        DrawDebugLine(
            GetWorld(), Start, End,
            bHit ? FColor::Red : FColor::Green,
            false, 1.f, 0, 1.f
        );
    }

    return bHit;
}

bool UTraversalComponent::TraceFromPoint(USceneComponent* StartPoint,const FVector& Direction,float Distance)
{
    if (!StartPoint)
    {
        return false;
    }

    FVector Start = StartPoint->GetComponentLocation();
    FVector End   = Start + Direction.GetSafeNormal() * Distance;

    FHitResult Hit;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    if (bDrawDebug)
    {
        DrawDebugLine(GetWorld(),Start,End,bHit ? FColor::Red : FColor::Green,false,2.f,0,2.f);

        if (bHit)
        {
            DrawDebugSphere(GetWorld(),Hit.ImpactPoint,8.f,8,FColor::Red,false,2.f);
        }
    }

    return bHit;
}

void UTraversalComponent::UpdateExitPoint(const FVector& Direction,float Offset)
{
    if (!CurrentTarget || !OwnerCharacter)
        return;

    UArrowComponent* ExitPoint = CurrentTarget->GetExitPoint();
    if (!ExitPoint)
        return;

    FVector BaseLocation =
        OwnerCharacter->GetCapsuleComponent()->GetComponentLocation();

    ExitPoint->SetWorldLocation(
        BaseLocation +
        Direction.GetSafeNormal() * Offset
    );
}



// ============================================================
// MONTAGE
// ============================================================

void UTraversalComponent::RequestMontage(
    float Input,
    bool bExiting,
    bool bEnter,
    bool bEntry,
    bool bExit)
{
    if (!CurrentTarget || bIsPlayingMontage) return;

    UAnimMontage* Montage = CurrentTarget->GetMontageForContext(
        Input, CurrentHand,
        bExiting, bEnter, bEntry, bExit
    );

    if (!Montage) return;

    PlayMontage(Montage);
}

void UTraversalComponent::PlayMontage(UAnimMontage* Montage)
{
    if (!OwnerCharacter || !Montage) return;

    UAnimInstance* Anim = OwnerCharacter->GetMesh()->GetAnimInstance();
    if (!Anim) return;

    bIsPlayingMontage = true;
    Anim->Montage_Play(Montage);

    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &UTraversalComponent::OnMontageCompleted);
    Anim->Montage_SetEndDelegate(EndDelegate, Montage);
}

void UTraversalComponent::OnMontageCompleted(UAnimMontage*, bool bInterrupted)
{
    bIsPlayingMontage = false;
}
