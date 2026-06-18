#pragma once

#include "CoreMinimal.h"
#include "InputReceiverInterface.h"
#include "Components/ActorComponent.h"
#include "TraversalInterface.h"
#include "TraversalComponent.generated.h"

class UContextDataAsset;
class UContextComponent;
class ATraversalActor;
class UAnimMontage;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class TRAVERSALSYSTEM_API UTraversalComponent : public UActorComponent, public IInputReceiverInterface
{
    GENERATED_BODY()

public:
    UTraversalComponent();

    virtual void BeginPlay() override;

    // IInputReceiverInterface
    virtual void HandleInput_Implementation(FGameplayTag Tag, const FInputActionValue& Value) override;

    // API publique
    void StartTraversal(ATraversalActor* Target);
    void StopTraversal();
    void ExitTraversal();

    void SetTraversalInput(FVector2D RawInput);
    void OnTraversalNotify(ETraversalNotifyType EventType);

    void SetCurrentHand(ETraversalHand Hand) { CurrentHand = Hand; }
    ETraversalHand GetCurrentHand()          const { return CurrentHand; }

    bool IsTraversing()     const { return State != ETraversalState::None; }
    bool IsPlayingMontage() const { return bIsPlayingMontage; }

    ATraversalActor* GetCurrentTarget() const { return CurrentTarget; }

    // Appelé par ContextTransitionComponent quand la transition est finie
    void OnTransitionFinished();

    // Montage
    void RequestMontage(
        float Input,
        bool bExiting  = false,
        bool bEnter    = false,
        bool bEntry    = false,
        bool bExit     = false);

    // Trace générique dans une direction
    bool TraceInDirection(const FVector& Direction, float Distance);
    bool TraceFromPoint(USceneComponent* StartPoint,const FVector& Direction,float Distance);
    void UpdateExitPoint(const FVector& Direction,float Offset);

    // ---- Config ----
    
    UPROPERTY(EditAnywhere, Category = "Traversal")
    float ApproachDuration = 0.25f;

    UPROPERTY(EditAnywhere, Category = "Traversal")
    float TraversalSpeed = 1.f;

    UPROPERTY(EditAnywhere, Category = "Traversal|Trace")
    float BoundsTraceLength = 50.f;

    UPROPERTY(EditAnywhere, Category = "Traversal|Trace")
    float VerticalTraceLength = 100.f;

    UPROPERTY(EditAnywhere, Category = "Traversal|Trace")
    float ExitZOffset = 77.f;

    UPROPERTY(EditAnywhere, Category = "Traversal|Debug")
    bool bDrawDebug = false;

    UPROPERTY(BlueprintReadOnly, Category = "Traversal")
    ETraversalHand CurrentHand = ETraversalHand::Right;

private:
    void SetMovementEnabled(bool bEnabled);
    void PlayMontage(UAnimMontage* Montage);
    void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

    UPROPERTY()
    ACharacter* OwnerCharacter = nullptr;

    UPROPERTY()
    ATraversalActor* CurrentTarget = nullptr;

    UPROPERTY()
    UContextComponent* ContextComponent = nullptr;

    ETraversalState State = ETraversalState::None;

    USceneComponent* EntryPoint = nullptr;

    bool bIsPlayingMontage = false;
};