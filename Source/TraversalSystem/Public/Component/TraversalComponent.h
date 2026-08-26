#pragma once

#include "CoreMinimal.h"
#include "ContextComponent.h"
#include "Interfaces/InputReceiverInterface.h"
#include "Components/ActorComponent.h"
#include "TraversalInterface.h"
#include "Data/ContextStructData.h"
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

    ACharacter* GetOwnerCharacter() const {return OwnerCharacter;};
    
    // IInputReceiverInterface
    virtual void HandleInput_Implementation(FGameplayTag Tag, const FInputActionValue& Value, ETriggerEvent TriggerEvent) override;

    // API publique
    void StartTraversal(ATraversalActor* Target);
    void RequestExitTraversal(UAnimMontage* ExitMontage);

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
    bool TraceInDirectionForObstacle(const FVector& Direction, float Distance);
    bool TraceFromPoint(FVector StartPoint, FVector EndPoint);
    void UpdateExitPoint(const FVector& Direction,float Offset);
    
    UPROPERTY(EditAnywhere, Category = "Traversal|Debug")
    bool bDrawDebug = false;

    UPROPERTY(BlueprintReadOnly, Category = "Traversal")
    ETraversalHand CurrentHand = ETraversalHand::Right;

private:
    void SetMovementEnabled(bool bEnabled);
    void PlayMontage(UAnimMontage* Montage);
    void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

    void OnContextStateChanged(const FActiveContext& Context, EContextState ContextState);

    
    void OnTraversalForcedRemoved(const FActiveContext& Context);

    UPROPERTY()
    ACharacter* OwnerCharacter = nullptr;

    UPROPERTY()
    ATraversalActor* CurrentTarget = nullptr;

    UPROPERTY()
    UContextComponent* ContextComponent = nullptr;

    ETraversalState State = ETraversalState::None;

    USceneComponent* EntryPoint = nullptr;

    bool bIsPlayingMontage = false;

    bool Exiting = false;

    FContextHandle CurrentContextHandle;
};