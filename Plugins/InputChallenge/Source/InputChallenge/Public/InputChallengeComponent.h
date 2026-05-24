#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputChallengeDefinition.h"
#include "InputChallengeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChallengeSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChallengeFailure);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChallengeProgress, float, Percent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChallengeStarted, UInputChallengeDefinition*, Definition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExpectedActionChanged, UInputAction*, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChallengeEnded);


UCLASS(ClassGroup=(InputChallenge), meta=(BlueprintSpawnableComponent))
class INPUTCHALLENGE_API UInputChallengeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInputChallengeComponent();

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

    // --- API ---
    UFUNCTION(BlueprintCallable, Category = "Challenge")
    void StartChallenge(UInputChallengeDefinition* Definition);

    UFUNCTION(BlueprintCallable, Category = "Challenge")
    void AbortChallenge();
    
    UFUNCTION(BlueprintPure, Category = "Challenge")
    bool IsActived() const { return bIsActive; }

    UFUNCTION(BlueprintPure, Category = "Challenge")
    float GetProgressPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Challenge")
    void HandleInputAction(UInputAction* Action);

    UFUNCTION(BlueprintPure, Category = "Challenge")
    UInputAction* GetCurrentExpectedAction() const;

    UFUNCTION(BlueprintPure, Category = "Challenge")
    float GetRemainingTime() const { return RemainingTime; }

protected:
    
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

private:
    void Succeed();
    void Fail();

    UPROPERTY()
    TObjectPtr<UInputChallengeDefinition> ActiveDefinition;

    bool  bIsActive      = false;
    int32 CurrentCount   = 0;
    int32 SequenceIndex  = 0;
    float RemainingTime  = 0.f;
};