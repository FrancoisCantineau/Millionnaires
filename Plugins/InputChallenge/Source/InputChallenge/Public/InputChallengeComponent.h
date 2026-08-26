#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputChallengeDefinition.h"
#include "Interfaces/InputReceiverInterface.h"
#include "InputChallengeComponent.generated.h"

class UInputChallengeWidget;



UENUM(BlueprintType)
enum class EInputChallengeState : uint8
{
    Idle,
    Running,
    Success,
    Failed
};




DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChallengeSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChallengeFailure);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChallengeProgress, float, Percent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChallengeStarted, UInputChallengeDefinition*, Definition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExpectedActionChanged, UInputAction*, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChallengeEnded);

/** Fires on a wrong input press, regardless of bResetOnMistake - lets Blueprint hook up
 *  feedback (a red flash, a buzz sound) for a mistake even when the challenge doesn't reset. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChallengeMistake);


UCLASS(ClassGroup=(InputChallenge), meta=(BlueprintSpawnableComponent))
class INPUTCHALLENGE_API UInputChallengeComponent : public UActorComponent, public IInputReceiverInterface
{
    GENERATED_BODY()

public:
    UInputChallengeComponent();

    //~ IInputReceiverInterface - lets this component receive input from ANY router plugin that
    //~ calls this shared interface (ContextFramework or otherwise) without InputChallenge ever
    //~ depending on that router's own types.
    virtual void HandleInput_Implementation(FGameplayTag Tag, const FInputActionValue& Value, ETriggerEvent TriggerEvent) override;
    //~ End IInputReceiverInterface
    
    // --- Delegates ---
    UPROPERTY(BlueprintAssignable, Category = "Challenge|Events")
    FOnChallengeSuccess OnSuccess;

    UPROPERTY(BlueprintAssignable, Category = "Challenge|Events")
    FOnChallengeFailure OnFailure;

    UPROPERTY(BlueprintAssignable, Category = "Challenge|Events")
    FOnChallengeProgress OnProgress;
    
    UPROPERTY(BlueprintAssignable, Category = "Challenge|Events")
    FOnChallengeStarted OnChallengeStarted;

    UPROPERTY(BlueprintAssignable, Category = "Challenge|Events")
    FOnChallengeEnded OnChallengeEnded;

    UPROPERTY(BlueprintAssignable, Category = "Challenge|Events")
    FOnExpectedActionChanged OnExpectedActionChanged;

    UPROPERTY(BlueprintAssignable, Category = "Challenge|Events")
    FOnChallengeMistake OnMistake;

    // --- API ---
    UFUNCTION(BlueprintCallable, Category = "Challenge")
    void StartChallenge(UInputChallengeDefinition* Definition);

    UFUNCTION(BlueprintCallable, Category = "Challenge")
    void AbortChallenge();
    
    UFUNCTION(BlueprintPure, Category = "Challenge")
    bool IsActived() const { return State == EInputChallengeState::Running; }

    UFUNCTION(BlueprintPure, Category = "Challenge")
    float GetProgressPercent() const;
    
    void HandleInputPressed(UInputAction* Action);
    
    void HandleInputReleased(UInputAction* Action);

    UFUNCTION(BlueprintPure, Category = "Challenge")
    UInputAction* GetCurrentExpectedAction() const;

    UFUNCTION(BlueprintPure, Category = "Challenge")
    float GetRemainingTime() const { return RemainingTime; }

    /** Optional. If set, StartChallenge automatically creates and shows this widget, and
     *  removes it when the challenge ends (success, failure, or abort) - no caller (an
     *  Orchestrator action, a quest objective, a door actor) ever needs to touch
     *  Create Widget/Add to Viewport itself, just call StartChallenge(Definition). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge|UI")
    TSubclassOf<UInputChallengeWidget> WidgetClass;

protected:
    
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

private:
    void Succeed();
    void Fail();

    /** Creates+shows WidgetClass if set, called from StartChallenge. */
    void CreateChallengeWidget();
    /** Removes and clears the auto-managed widget, if any - called from every path that ends
     *  the challenge (Succeed, Fail, AbortChallenge), so it never lingers on screen. */
    void CleanupChallengeWidget();
    /** Restores normal game speed if this challenge had slow-motion enabled. Called from every
     *  end path (Succeed, Fail, AbortChallenge) - same reasoning as CleanupChallengeWidget:
     *  never leave the world stuck in slow-motion no matter how the challenge ended. */
    void RestoreTimeDilation();

    /** Resolves an incoming Tag (from HandleInput_Implementation) back to the paired
     *  UInputAction, via ActiveDefinition->ExpectedInputs - the only place InputChallenge and
     *  whatever router feeds it ever need to agree on anything. Null if no match. */
    UInputAction* ResolveActionFromTag(FGameplayTag Tag) const;

    /** Broadcasts Request.Context.Push for Context.Challenge on the GameplayEventBus, so
     *  whatever context-routing plugin is listening (ContextFramework or otherwise) takes over
     *  input priority for this challenge. Called once, from StartChallenge. InputChallenge never
     *  includes anything from that plugin - it only knows the tag and the bus. */
    void PushChallengeContext();

    /** Broadcasts Request.Context.Pop for Context.Challenge. Called from EVERY path that ends
     *  the challenge (Succeed, Fail, AbortChallenge) - same reasoning as
     *  CleanupChallengeWidget/RestoreTimeDilation: never leave a context pushed with nobody
     *  left to pop it. */
    void PopChallengeContext();

    UPROPERTY()
    TObjectPtr<UInputChallengeDefinition> ActiveDefinition;

    UPROPERTY()
    TObjectPtr<UInputChallengeWidget> ActiveWidget;

    int32 CurrentCount   = 0;
    int32 SequenceIndex  = 0;
    float RemainingTime  = 0.f;

    EInputChallengeState State =
        EInputChallengeState::Idle;

    float CurrentHoldTime = 0.f;

    bool bHolding = false;
};