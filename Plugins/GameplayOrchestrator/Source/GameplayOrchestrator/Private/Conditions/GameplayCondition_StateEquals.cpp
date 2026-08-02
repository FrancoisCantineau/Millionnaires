#include "Conditions/GameplayCondition_StateEquals.h"
#include "State/GameplayWorldStateSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

bool UGameplayCondition_StateEquals::Evaluate_Implementation(const FEventContext& Context)
{
	UWorld* World = Context.Sender.IsValid() ? Context.Sender->GetWorld() : (GEngine ? GEngine->GetCurrentPlayWorld() : nullptr);
	if (!World)
	{
		return false;
	}
	if (UGameInstance* GameInstance = World->GetGameInstance())
	{
		if (UGameplayWorldStateSubsystem* StateSubsystem = GameInstance->GetSubsystem<UGameplayWorldStateSubsystem>())
		{
			return StateSubsystem->GetState(StateKey) == ExpectedValue;
		}
	}
	return false;
}

#if WITH_EDITOR
FString UGameplayCondition_StateEquals::GetEditorSummary() const
{
	return FString::Printf(TEXT("State %s == %s"), *StateKey.ToString(), *ExpectedValue.ToString());
}
#endif
