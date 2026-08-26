#include "InputChallengeComponent.h"
#include "UI/InputChallengeWidget.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Event/GameplayEventBus.h"
#include "Core/GameplayEventContext.h"
#include "Tags/CoreGameplayTags.h"

UInputChallengeComponent::UInputChallengeComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UInputChallengeComponent::StartChallenge(UInputChallengeDefinition* Definition)
{
    if (!Definition) return;

    if (State == EInputChallengeState::Running)
    {
        AbortChallenge(); // <- si AbortChallenge fait bien le Pop (voir plus bas), l'ordre est correct :
                           //    on quitte l'ancien contexte AVANT de pousser le nouveau
    }

    ActiveDefinition = Definition;
    CurrentCount     = 0;
    SequenceIndex    = 0;
    RemainingTime    = Definition->TimeLimit;
    State = EInputChallengeState::Running;
    CurrentHoldTime = 0.f;
    bHolding = false;

    OnChallengeStarted.Broadcast(Definition);
    OnProgress.Broadcast(0.f);
    OnExpectedActionChanged.Broadcast(GetCurrentExpectedAction());

    CreateChallengeWidget();

    if (Definition->bUseSlowMotion && GetWorld())
    {
        UGameplayStatics::SetGlobalTimeDilation(GetWorld(), Definition->SlowMotionScale);
    }

    PushChallengeContext();
}

void UInputChallengeComponent::PushChallengeContext()
{
    UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    UGameplayEventBus* Bus = GameInstance ? GameInstance->GetSubsystem<UGameplayEventBus>() : nullptr;
    if (!Bus) return;

    FEventContext Ctx = FEventContext::Make(TAG_Request_Context_Push, GetOwner(), GetOwner());
    Ctx.AdditionalTags.AddTag(TAG_Context_Challenge);
    Ctx.Payload = this;
    Bus->Broadcast(TAG_Request_Context_Push, Ctx);
}

void UInputChallengeComponent::PopChallengeContext()
{
    UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    UGameplayEventBus* Bus = GameInstance ? GameInstance->GetSubsystem<UGameplayEventBus>() : nullptr;
    if (!Bus) return;

    FEventContext Ctx = FEventContext::Make(TAG_Request_Context_Pop, GetOwner(), GetOwner());
    Ctx.AdditionalTags.AddTag(TAG_Context_Challenge);
    Bus->Broadcast(TAG_Request_Context_Pop, Ctx);
}

void UInputChallengeComponent::CreateChallengeWidget()
{
    // The definition's own style takes priority - lets a specific challenge look different
    // from the component's default without the caller ever choosing a widget class.
    TSubclassOf<UInputChallengeWidget> ClassToUse = (ActiveDefinition && ActiveDefinition->WidgetClassOverride)
        ? ActiveDefinition->WidgetClassOverride : WidgetClass;

    if (!ClassToUse)
    {
        return;
    }

    // Resolve a PlayerController regardless of whether this component lives on a Pawn or a
    // PlayerController directly (either is a reasonable place to put it - see design discussion).
    APlayerController* PC = nullptr;
    if (APlayerController* AsPC = Cast<APlayerController>(GetOwner()))
    {
        PC = AsPC;
    }
    else if (APawn* AsPawn = Cast<APawn>(GetOwner()))
    {
        PC = Cast<APlayerController>(AsPawn->GetController());
    }

    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("UInputChallengeComponent::CreateChallengeWidget - couldn't resolve a PlayerController from '%s'. WidgetClass won't be shown."), GetOwner() ? *GetOwner()->GetName() : TEXT("(no owner)"));
        return;
    }

    ActiveWidget = CreateWidget<UInputChallengeWidget>(PC, ClassToUse);
    if (ActiveWidget)
    {
        ActiveWidget->Init(this);
        ActiveWidget->AddToViewport();
    }
}

void UInputChallengeComponent::CleanupChallengeWidget()
{
    if (ActiveWidget)
    {
        ActiveWidget->RemoveFromParent();
        ActiveWidget = nullptr;
    }
}

void UInputChallengeComponent::RestoreTimeDilation()
{
    if (ActiveDefinition && ActiveDefinition->bUseSlowMotion && GetWorld())
    {
        UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
    }
}

void UInputChallengeComponent::AbortChallenge()
{
    if (State != EInputChallengeState::Running) return;

    State = EInputChallengeState::Idle;
    bHolding = false;
    CleanupChallengeWidget();
    RestoreTimeDilation();
    PopChallengeContext();
    OnChallengeEnded.Broadcast();
}

void UInputChallengeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (State != EInputChallengeState::Running || !ActiveDefinition)
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
        return FMath::Clamp((float)SequenceIndex / ActiveDefinition->ExpectedInputs.Num(), 0.f, 1.f);
    case EInputChallengeType::Hold:
        return ActiveDefinition->HoldDuration > 0.f
            ? FMath::Clamp(CurrentHoldTime / ActiveDefinition->HoldDuration, 0.f, 1.f) : 0.f;
    default:
        return 0.f;
    }
}


void UInputChallengeComponent::HandleInputPressed(UInputAction* Action)
{
    if (State != EInputChallengeState::Running || !ActiveDefinition || !Action)
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
            else
            {
                OnMistake.Broadcast();
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

                if (SequenceIndex >= ActiveDefinition->ExpectedInputs.Num())
                {
                    Succeed();
                }
            }
            else
            {
                OnMistake.Broadcast();

                if (ActiveDefinition->bResetOnMistake)
                {
                    SequenceIndex = 0;

                    OnProgress.Broadcast(0.f);

                    OnExpectedActionChanged.Broadcast(
                        GetCurrentExpectedAction()
                    );
                }
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
    if (State != EInputChallengeState::Running || !ActiveDefinition) return;

    // Release only means something for a Hold challenge - Spam/Sequence never set bHolding,
    // so CurrentHoldTime stays at 0.f for them. Without this guard, a mistakenly non-zero
    // HoldDuration left on a Spam/Sequence definition would auto-Fail the challenge the moment
    // the player releases the expected key, even though nothing was ever supposed to be held.
    if (ActiveDefinition->Type != EInputChallengeType::Hold)
        return;

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
    if (State != EInputChallengeState::Running || !ActiveDefinition) return nullptr;
    switch (ActiveDefinition->Type)
    {
    case EInputChallengeType::Spam:
    case EInputChallengeType::Hold:
        return ActiveDefinition->ExpectedInputs.IsValidIndex(0)
            ? ActiveDefinition->ExpectedInputs[0].Action : nullptr;
    case EInputChallengeType::Sequence:
        return ActiveDefinition->ExpectedInputs.IsValidIndex(SequenceIndex)
            ? ActiveDefinition->ExpectedInputs[SequenceIndex].Action : nullptr;
    default:
        return nullptr;
    }
}

UInputAction* UInputChallengeComponent::ResolveActionFromTag(FGameplayTag Tag) const
{
    if (!ActiveDefinition || !Tag.IsValid())
    {
        return nullptr;
    }

    for (const FExpectedInputEntry& Entry : ActiveDefinition->ExpectedInputs)
    {
        if (Entry.Tag == Tag)
        {
            return Entry.Action;
        }
    }
    return nullptr;
}

void UInputChallengeComponent::HandleInput_Implementation(FGameplayTag Tag, const FInputActionValue& Value, ETriggerEvent TriggerEvent)
{
    UInputAction* Action = ResolveActionFromTag(Tag);
    if (!Action)
    {
        return;
    }

    switch (TriggerEvent)
    {
    case ETriggerEvent::Started:
        HandleInputPressed(Action);
        break;
	case ETriggerEvent::Completed:
        HandleInputReleased(Action);
        break;

	case ETriggerEvent::Triggered:
        HandleInputPressed(Action);
        break;
    case ETriggerEvent::Canceled:
        HandleInputReleased(Action);
        break;
    default:
        break;
    }
}



void UInputChallengeComponent::Succeed()
{
    State = EInputChallengeState::Success;

    bHolding = false;
    CleanupChallengeWidget();
    RestoreTimeDilation();
    
    PopChallengeContext();

    OnSuccess.Broadcast();
    OnChallengeEnded.Broadcast();
}

void UInputChallengeComponent::Fail()
{
    State = EInputChallengeState::Failed;

    bHolding = false;
    CleanupChallengeWidget();
    RestoreTimeDilation();
    
    PopChallengeContext();

    OnFailure.Broadcast();
    OnChallengeEnded.Broadcast();
}