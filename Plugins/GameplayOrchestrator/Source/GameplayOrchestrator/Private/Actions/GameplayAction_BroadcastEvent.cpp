#include "Actions/GameplayAction_BroadcastEvent.h"
#include "Event/GameplayEventBus.h"
#include "Core/GameplayEventContext.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"


void UGameplayAction_BroadcastEvent::Execute_Implementation(const FEventContext& Context)
{
	if (!EventTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameplayAction_BroadcastEvent] EventTag not set, nothing broadcast."));
		return;
	}

	AActor* SenderActor = Context.Sender.Get();
	const UWorld* World = SenderActor ? SenderActor->GetWorld() : nullptr;
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	UGameplayEventBus* Bus = GameInstance ? GameInstance->GetSubsystem<UGameplayEventBus>() : nullptr;

	if (!Bus)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameplayAction_BroadcastEvent] GameplayEventBus not found, '%s' was not broadcast."), *EventTag.ToString());
		return;
	}

	FEventContext OutCtx = FEventContext::Make(EventTag, SenderActor, Context.Instigator.Get());
	OutCtx.AdditionalTags = AdditionalTags;
	OutCtx.Payload = PayloadObject;

	Bus->Broadcast(EventTag, OutCtx);
}
