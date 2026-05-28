#include "InputChallengeComponent.h"

UInputChallengeComponent::UInputChallengeComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UInputChallengeComponent::StartChallenge(UInputChallengeDefinition* Definition)
{
    if (!Definition) return;

    ActiveDefinition = Definition;
    bIsActive        = true;
    CurrentCount     = 0;
    SequenceIndex    = 0;
    RemainingTime    = Definition->TimeLimit;
    State = EInputChallengeState::Running;
    CurrentHoldTime = 0.f;
    bHolding = false;

    OnChallengeStarted.Broadcast(Definition);
    OnProgress.Broadcast(0.f);
    OnExpectedActionChanged.Broadcast(GetCurrentExpectedAction());
}

void UInputChallengeComponent::AbortChallenge()
{
    if (!bIsActive) return;

    bIsActive = false;
    OnChallengeEnded.Broadcast();
}

void UInputChallengeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!bIsActive || !ActiveDefinition)
        return;

    if (ActiveDefinition->TimeLimit > 0.f)
    {
        RemainingTime -= DeltaTime;

        if (RemainingTime <= 0.f)
        {
            Fail();
            return;
        }
    }

    if (ActiveDefinition->Type != EInputChallengeType::Hold)
        return;

    if (!bHolding)
        return;

    CurrentHoldTime += DeltaTime;

    float Progress =
        CurrentHoldTime /
        ActiveDefinition->HoldDuration;

    OnProgress.Broadcast(
        FMath::Clamp(Progress, 0.f, 1.f)
    );

    if (Progress >= 1.f)
    {
        Succeed();
    }
}

float UInputChallengeComponent::GetProgressPercent() const
{
    if (!ActiveDefinition) return 0.f;
    switch (ActiveDefinition->Type)
    {
    case EInputChallengeType::Spam:
        return FMath::Clamp((float)CurrentCount / ActiveDefinition->RequiredCount, 0.f, 1.f);
    case EInputChallengeType::Sequence:
        return FMath::Clamp((float)SequenceIndex / ActiveDefinition->ExpectedActions.Num(), 0.f, 1.f);
    default:
        return 0.f;
    }
}


void UInputChallengeComponent::HandleInputPressed(UInputAction* Action)
{
    if (!bIsActive || !ActiveDefinition || !Action)
        return;

    switch (ActiveDefinition->Type)
    {
    case EInputChallengeType::Spam:
        {
            if (Action == GetCurrentExpectedAction())
            {
                CurrentCount++;

                OnProgress.Broadcast(GetProgressPercent());

                if (CurrentCount >= ActiveDefinition->RequiredCount)
                {
                    Succeed();
                }
            }
            break;
        }

    case EInputChallengeType::Sequence:
        {
            if (Action == GetCurrentExpectedAction())
            {
                SequenceIndex++;

                OnProgress.Broadcast(GetProgressPercent());

                OnExpectedActionChanged.Broadcast(
                    GetCurrentExpectedAction()
                );

                if (SequenceIndex >= ActiveDefinition->ExpectedActions.Num())
                {
                    Succeed();
                }
            }
            else if (ActiveDefinition->bResetOnMistake)
            {
                SequenceIndex = 0;

                OnProgress.Broadcast(0.f);

                OnExpectedActionChanged.Broadcast(
                    GetCurrentExpectedAction()
                );
            }

            break;
        }

    case EInputChallengeType::Hold:
        {
            if (Action == GetCurrentExpectedAction())
            {
                bHolding = true;
                CurrentHoldTime = 0.f;
            }

            break;
        }
    }
}

void UInputChallengeComponent::HandleInputReleased(UInputAction* Action)
{
    if (!ActiveDefinition) return;

    if (Action != GetCurrentExpectedAction())
        return;

    if (CurrentHoldTime <
        ActiveDefinition->HoldDuration)
    {
        Fail();
        return;
    }

    bHolding = false;
}

UInputAction* UInputChallengeComponent::GetCurrentExpectedAction() const
{
    if (!bIsActive || !ActiveDefinition) return nullptr;
    switch (ActiveDefinition->Type)
    {
    case EInputChallengeType::Spam:
        return ActiveDefinition->ExpectedActions.IsValidIndex(0)
            ? ActiveDefinition->ExpectedActions[0] : nullptr;
    case EInputChallengeType::Sequence:
        return ActiveDefinition->ExpectedActions.IsValidIndex(SequenceIndex)
            ? ActiveDefinition->ExpectedActions[SequenceIndex] : nullptr;

    case EInputChallengeType::Hold:
        return ActiveDefinition->ExpectedActions.IsValidIndex(0)
            ? ActiveDefinition->ExpectedActions[0] : nullptr;
        
    default:
        return nullptr;
    }
}



void UInputChallengeComponent::Succeed()
{
    State = EInputChallengeState::Success;
    
    bIsActive = false;
    bHolding = false;
    
    OnSuccess.Broadcast();
    OnChallengeEnded.Broadcast();
}

void UInputChallengeComponent::Fail()
{
    State = EInputChallengeState::Failed;

    bIsActive = false;
    bHolding = false;
    
    OnFailure.Broadcast();
    OnChallengeEnded.Broadcast();
}