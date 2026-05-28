#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TraversalInterface.h"
#include "TraversalComponent.generated.h"

class ATraversalActor;
class UAnimMontage;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class TRAVERSALSYSTEM_API UTraversalComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTraversalComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float, ELevelTick, FActorComponentTickFunction*) override;

    // API publique
    void StartTraversal(ATraversalActor* Target);
    void StopTraversal();
    void SetTraversalInput(FVector2D RawInput);
    void OnTraversalNotify(ETraversalNotifyType EventType);
    void SetCurrentHand(ETraversalHand Hand) { CurrentHand = Hand; }

    bool IsTraversing() const { return State != ETraversalState::None; }

    UPROPERTY(BlueprintReadOnly, Category = "Traversal")
    ETraversalHand CurrentHand = ETraversalHand::Right;

private:
    // State machine
    void UpdateApproach(float DeltaTime);
    void EnterTraversal();
    void UpdateTraversal();
    void ExitTraversal();

    // Movement
    void SetMovementMode(bool bTraversing);

    // Trace
    // Montage
    void RequestMontage(float Input, bool bExiting = false, bool bEnter = false, bool bEntry = false, bool bExit = false);
    void PlayMontage(UAnimMontage* Montage);
    void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

    UPROPERTY()
    ACharacter* OwnerCharacter = nullptr;

private:
    UPROPERTY()
    ATraversalActor* CurrentTarget = nullptr;

    ETraversalState State = ETraversalState::None;

    // Approach
    FTransform ApproachStartTransform;
    float ApproachElapsed = 0.f;

    UPROPERTY(EditAnywhere, Category = "Traversal")
    float ApproachDuration = 0.25f;

    // Traversal
    float Alpha = 0.f;
    float TraversalInput = 0.f;

    UPROPERTY(EditAnywhere, Category = "Traversal")
    float TraversalSpeed = 1.f;

    // Bounds
    bool bCanMoveForward  = true;
    bool bCanMoveBackward = true;
    bool bCanExitForward  = false;
    bool bCanExitBackward = false;

    UPROPERTY(EditAnywhere, Category = "Traversal|Debug")
    float BoundsTraceLength = 50.f;

    // Montage
    bool bIsPlayingMontage = false;

    // Debug
    UPROPERTY(EditAnywhere, Category = "Traversal|Debug")
    bool bDrawDebug = false;



    bool  TraceVertical(float InputDir);          // true = obstacle détecté
    void  UpdateExitPoint();                       // repositionne l'exit point
    bool  TraceExitForward();                      // trace depuis exit point

    // Paramètres de trace (expose en UPROPERTY si besoin)
    UPROPERTY(EditAnywhere, Category="Traversal|Trace")
    float VerticalTraceLength = 100.f;            // longueur du trace vertical

    UPROPERTY(EditAnywhere, Category="Traversal|Trace")
    float ExitZOffset = 77.f;
};