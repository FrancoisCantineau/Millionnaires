#include "Component/TraversalComponent.h"

#include "TraversalActor.h"
#include "ContextComponent.h"
#include "ContextTransitionComponent.h"
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

    if (ContextComponent)
    {
        ContextComponent->OnContextStateChanged.AddUObject(this,&UTraversalComponent::OnContextStateChanged);
        ContextComponent->OnContextRemoved.AddUObject(this, &UTraversalComponent::OnTraversalForcedRemoved);
    }
}

// ============================================================
// START
// ============================================================

void UTraversalComponent::StartTraversal(ATraversalActor* Target)
{
    if (!Target || !OwnerCharacter) return;

    CurrentTarget = Target;

    FTraversalEntryInfo EntryInfo = Target->GetEntryInfo(OwnerCharacter, this);
    EntryPoint = EntryInfo.EntryPoint;

    SetMovementEnabled(false);
    OwnerCharacter->bUseControllerRotationYaw = false;
    OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
    OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    State = ETraversalState::Approaching;

    if (ContextComponent)
    {
        FActiveContext Context;
        Context.Definition      = Target->GetContextData();
        Context.Source          = Target;
        Context.SnapTarget      = EntryInfo.EntryPoint;
        Context.InputReceiver   = this;
        Context.TransitionData.EnterMontage = EntryInfo.EnterTransition;

        CurrentContextHandle = ContextComponent->AddContext(Context);
    }
    OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
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

void UTraversalComponent::RequestExitTraversal(UAnimMontage* ExitMontage)
{
    if (Exiting || bIsPlayingMontage)
        return;
    
    UContextTransitionComponent* Transition =
    GetOwner()->FindComponentByClass<UContextTransitionComponent>();

    if (!Transition)
        return;


    Exiting = true;

    Transition->RequestContextExit(
        CurrentContextHandle,
        ExitMontage);
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
    if (State == ETraversalState::Traversing)
    {
        FVector2D MoveVector = Value.Get<FVector2D>();
        SetTraversalInput(MoveVector);
    }
   
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

bool UTraversalComponent::TraceInDirectionForObstacle(const FVector& Direction, float Distance)
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

bool UTraversalComponent::TraceFromPoint(FVector StartPoint, FVector EndPoint)
{
    FHitResult Hit;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        StartPoint,
        EndPoint,
        ECC_Visibility,
        Params
    );

    if (bDrawDebug)
    {
        DrawDebugLine(GetWorld(),StartPoint,EndPoint,bHit ? FColor::Red : FColor::Green,false,2.f,0,2.f);

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
    if (!CurrentTarget || bIsPlayingMontage || Exiting) return;

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

void UTraversalComponent::OnContextStateChanged(const FActiveContext& Context, EContextState ContextState)
{
    if (Context.Handle != CurrentContextHandle)
        return;

    switch (ContextState)
    {
    case EContextState::Active:
        OnTransitionFinished();
        break;

    case EContextState::Exiting:
        State = ETraversalState::None;
        break;

    case EContextState::StartExit:
        State = ETraversalState::None;
        break;
    }
}

void UTraversalComponent::OnTraversalForcedRemoved(const FActiveContext& Context)
{
    if (Context.Handle == CurrentContextHandle)
    {
        CurrentContextHandle = FContextHandle();
        CurrentTarget = nullptr;
        Exiting = false;


       OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
  
    }
}

