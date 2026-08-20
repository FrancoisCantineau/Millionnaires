#include "Component/GameplayEventEmitterComponent.h"
#include "Event/GameplayEventBus.h"

void UGameplayEventEmitterComponent::Publish()
{
	PublishTag(DefaultEventTag);
}

void UGameplayEventEmitterComponent::PublishTag(FGameplayTag EventTag)
{
	PublishTagWithInstigator(EventTag, GetOwner());
}

void UGameplayEventEmitterComponent::PublishTagWithInstigator(FGameplayTag EventTag, AActor* Instigator)
{
	if (!EventTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("UGameplayEventEmitterComponent::PublishTagWithInstigator - called with an invalid tag on '%s'"), GetOwner() ? *GetOwner()->GetName() : TEXT("(no owner)"));
		return;
	}

	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	if (!GameInstance)
	{
		return;
	}

	if (UGameplayEventBus* Bus = GameInstance->GetSubsystem<UGameplayEventBus>())
	{
		Bus->PublishEvent(EventTag, GetOwner(), Instigator);
	}
}
