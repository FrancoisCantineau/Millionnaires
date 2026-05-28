#include "Component/TraversalComponent.h"
#include "TraversalActor.h"
#include "DrawDebugHelpers.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Animation/AnimInstance.h"
#include "Character/CharacterInterface.h"

UTraversalComponent::UTraversalComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UTraversalComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UTraversalComponent::TickComponent(
    float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (State == ETraversalState::Approaching)
        UpdateApproach(DeltaTime);
}

// ============================================================
// API
// ============================================================

void UTraversalComponent::StartTraversal(ATraversalActor* Target)
{
    if (!Target) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    CurrentTarget       = Target;
    ApproachStartTransform = Owner->GetActorTransform();
    ApproachElapsed     = 0.f;
    Alpha               = 0.f;
    TraversalInput      = 0.f;
    State               = ETraversalState::Approaching;
}

void UTraversalComponent::StopTraversal()
{
    ExitTraversal();
    State          = ETraversalState::None;
    TraversalInput = 0.f;
    CurrentTarget  = nullptr;
}

void UTraversalComponent::SetTraversalInput(FVector2D RawInput)
{
    if (!CurrentTarget) return;
    
    TraversalInput = FMath::Clamp(RawInput.Y, -1.f, 1.f);

    UpdateTraversal();
}

void UTraversalComponent::OnTraversalNotify(ETraversalNotifyType EventType)
{
    if (!CurrentTarget) return;

    // L'actor gere sa propre logique de notify
    CurrentTarget->HandleTraversalNotify(EventType, this);
}

// ============================================================
// STATE MACHINE
// ============================================================


void UTraversalComponent::UpdateApproach(float DeltaTime)
{
    AActor* Owner = GetOwner();
    if (!Owner || !CurrentTarget) return;

    ApproachElapsed += DeltaTime;
    float T = FMath::Clamp(ApproachElapsed / ApproachDuration, 0.f, 1.f);

    FVector NewLoc = FMath::Lerp(
        ApproachStartTransform.GetLocation(),
        CurrentTarget->GetStartPoint()->GetComponentLocation(),
        T
    );

    FRotator NewRot = FMath::Lerp(
        ApproachStartTransform.GetRotation().Rotator(),
        CurrentTarget->GetStartPoint()->GetComponentRotation(),
        T
    );

    Owner->SetActorLocationAndRotation(NewLoc, NewRot, true);

    if (T >= 1.f)
    {
        EnterTraversal();
    }
}

void UTraversalComponent::EnterTraversal()
{
    SetMovementMode(true);

    Alpha = 0.f;

    if (GetOwner()->Implements<UCharacterInterface>())
    {
        ICharacterInterface::Execute_AddStateTag(
            GetOwner(),
            FGameplayTag::RequestGameplayTag(FName("State.Traversal.Ladder")));
        
    }
    
    State = ETraversalState::Traversing;

    if (!OwnerCharacter) return;
    
    RequestMontage(0.f, false, true, true, false);
}

void UTraversalComponent::UpdateTraversal()
{
    if (bIsPlayingMontage) return;
    
    if (FMath::Abs(TraversalInput) < 0.1f)
        return;

    bool bObstacle = TraceVertical(TraversalInput);

    if (TraversalInput < 0.f)
    {
        if (bObstacle)
        {
            RequestMontage(-1.f, true, false, true, false);
            ExitTraversal();
        }
        else
        {
            RequestMontage(TraversalInput, false);
        }
        return;
    }
    if (bObstacle)
    {
        return;
    }
    
    UpdateExitPoint();

    if (TraceExitForward())
    {
        RequestMontage(1.f, false, false, false, false);
    }
    else
    {
        RequestMontage(1.f, true, false, false, true);
        ExitTraversal();
    }
    
    
    
    /*if (!CurrentTarget || bIsPlayingMontage) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    UpdateBounds();
    UpdateExitAvailability();

    float Input = TraversalInput;
    if (Input > 0.f && !bCanMoveForward)  Input = 0.f;
    if (Input < 0.f && !bCanMoveBackward) Input = 0.f;

    Alpha = FMath::Clamp(
        Alpha + Input * DeltaTime * TraversalSpeed,
        0.f, 1.f
    );

    // Deplacement le long de l'axe
    FVector NewLoc = FMath::Lerp(
        CurrentTarget->GetStartPoint()->GetComponentLocation(),
        CurrentTarget->GetEndPoint()->GetComponentLocation(),
        Alpha
    );

    UE_LOG(LogTemp, Display, TEXT("NewLoc: %s"), *NewLoc.ToString());
    
    UE_LOG(LogTemp, Display, TEXT("Alpha: %f"), Alpha);
    Owner->SetActorLocation(NewLoc, true);

    // Sorties
    if (Alpha >= 1.f && bCanExitForward)
    {
        RequestMontage(1.f, true);
    }
    else if (Alpha <= 0.f && bCanExitBackward)
    {
        RequestMontage(-1.f, true);
    }*/
}

// ============================================================
// TRACE
// ============================================================



bool UTraversalComponent::TraceExitForward()
{
    if (!CurrentTarget) return false;

    USceneComponent* ExitPoint = CurrentTarget->GetExitPoint();
    if (!ExitPoint) return false;

    FVector Start   = ExitPoint->GetComponentLocation();
    FVector Forward = ExitPoint->GetForwardVector();
    FVector End     = Start + Forward * 100.f;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit, Start, End, ECC_Visibility, Params
    );

    if (bDrawDebug)
    {
        DrawDebugLine(
            GetWorld(), Start, End,
            bHit ? FColor::Blue : FColor::Orange,
            false, 2.f, 0, 2.f
        );
        if (bHit)
            DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 8.f, 8, FColor::Blue, false, 2.f);

        // Label debug au-dessus de l'exit point
        DrawDebugString(
            GetWorld(), Start + FVector(0, 0, 20),
            bHit ? TEXT("ExitFwd: HIT") : TEXT("ExitFwd: MISS"),
            nullptr, bHit ? FColor::Blue : FColor::Orange,
            10.f
        );
    }

    return bHit;
}

bool UTraversalComponent::TraceVertical(float InputDir)
{
    AActor* Owner = GetOwner();
    if (!Owner && !OwnerCharacter) return false;

    // Point de départ = centre de la capsule
    FVector Start = OwnerCharacter->GetCapsuleComponent()->GetComponentLocation();

    // Direction : vers le haut si on monte, vers le bas si on descend
    FVector Dir = (InputDir > 0.f) ? FVector::UpVector : FVector::DownVector;
    FVector End = Start + Dir * VerticalTraceLength;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit, Start, End, ECC_Visibility, Params
    );

    if (bDrawDebug)
    {
        DrawDebugLine(
            GetWorld(), Start, End,
            bHit ? FColor::Red : FColor::Green,
            false, 10.f, 0, 2.f
        );
        // Sphère au point d'impact si touché
        if (bHit)
            DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 8.f, 8, FColor::Red, false, 2.f);
    }

    return bHit;
}

// ------------------------------------------------------------
// Repositionne l'exit point :
//   X, Y  = position de la capsule du joueur
//   Z     = Z de l'UpArrow + ExitZOffset (77 dans le Blueprint)
// ------------------------------------------------------------
void UTraversalComponent::UpdateExitPoint()
{
    if (!CurrentTarget &&!OwnerCharacter ) return;

    USceneComponent* ExitPoint = CurrentTarget->GetExitPoint();
    if (!ExitPoint) return;

    // Capsule XY
    FVector CapsuleLoc = OwnerCharacter->GetCapsuleComponent()->GetComponentLocation();

    USceneComponent* UpArrow = CurrentTarget->GetExitPoint(); 
    if (!UpArrow) return;

    FVector NewExitLoc = FVector(
        CapsuleLoc.X,
        CapsuleLoc.Y,
        CapsuleLoc.Z + ExitZOffset
    );

    ExitPoint->SetWorldLocation(NewExitLoc);

    if (bDrawDebug)
    {
        DrawDebugSphere(
            GetWorld(), NewExitLoc, 10.f, 8,
            FColor::Yellow, false, 2.f
        );
    }
}

void UTraversalComponent::ExitTraversal()
{
    State = ETraversalState::Exiting; 
    CurrentTarget = nullptr;

    if (GetOwner()->Implements<UCharacterInterface>())
    {
        ICharacterInterface::Execute_RemoveStateTag(
            GetOwner(),
            FGameplayTag::RequestGameplayTag(FName("State.Traversal.Ladder")));
        
    }
}
// ============================================================
// MOVEMENT MODE
// ============================================================

void UTraversalComponent::SetMovementMode(bool bTraversing)
{
    if (!OwnerCharacter) return;

    auto* Move = OwnerCharacter->GetCharacterMovement();

    if (bTraversing)
    {
        Move->SetMovementMode(MOVE_Flying);
        Move->GravityScale  = 0.f;
        Move->Velocity      = FVector::ZeroVector;
    }
    else
    {
        Move->SetMovementMode(MOVE_Walking);
        Move->GravityScale = 1.f;
    }
}

// ============================================================
// MONTAGE
// ============================================================

void UTraversalComponent::RequestMontage(float Input, bool bExiting, bool bEnter, bool bEntry, bool bExit)
{
    if (!CurrentTarget || bIsPlayingMontage) return;

    UAnimMontage* Montage = CurrentTarget->GetMontageForContext(
        Input,
        CurrentHand,
        bExiting,
        bEnter, bEntry, bExit
    );

    PlayMontage(Montage);
}

void UTraversalComponent::PlayMontage(UAnimMontage* Montage)
{
    if (!Montage) return;
    
    if (!OwnerCharacter) return;

    UAnimInstance* Anim = OwnerCharacter->GetMesh()->GetAnimInstance();
    if (!Anim) return;

    bIsPlayingMontage = true;

    Anim->Montage_Play(Montage);

    FOnMontageEnded End;
    End.BindUObject(this, &UTraversalComponent::OnMontageCompleted);
    Anim->Montage_SetEndDelegate(End, Montage);
}

void UTraversalComponent::OnMontageCompleted(
    UAnimMontage* Montage, bool bInterrupted
)
{
    bIsPlayingMontage = false;

    if (bInterrupted)
    {
        SetMovementMode(false);
        State         = ETraversalState::None;
        CurrentTarget = nullptr;
        return;
    }
    
    if (State == ETraversalState::Exiting)
    {
        SetMovementMode(false);
        State         = ETraversalState::None;
        CurrentTarget = nullptr;
        return;
    }
    
    if (State == ETraversalState::Traversing && FMath::Abs(TraversalInput) > 0.1f)
    {
        UpdateTraversal();
    }
}