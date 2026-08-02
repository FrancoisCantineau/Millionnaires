#include "Event/GameplayEventBus.h"

void UGameplayEventBus::Broadcast(FGameplayTag EventTag, const FEventContext& Context)
{
	if (const FNativeSubscribers* Subscribers = NativeSubscribersByTag.Find(EventTag))
	{
		// Copie défensive : un callback peut se désabonner pendant l'itération.
		TArray<TFunction<void(const FEventContext&)>> CallbacksCopy;
		Subscribers->Callbacks.GenerateValueArray(CallbacksCopy);
		for (const TFunction<void(const FEventContext&)>& Callback : CallbacksCopy)
		{
			Callback(Context);
		}
	}

	// Diffusion Blueprint : TOUS les events, sans filtrage par tag (voir
	// commentaire sur OnEventReceived dans le header pour la raison).
	OnEventReceived.Broadcast(Context);
}

void UGameplayEventBus::PublishEvent(FGameplayTag EventTag, AActor* Sender, AActor* Instigator)
{
	Broadcast(EventTag, FEventContext::Make(EventTag, Sender, Instigator));
}

FDelegateHandle UGameplayEventBus::SubscribeNative(FGameplayTag EventTag, TFunction<void(const FEventContext&)> Callback)
{
	FDelegateHandle Handle = FDelegateHandle(FDelegateHandle::GenerateNewHandle);
	NativeSubscribersByTag.FindOrAdd(EventTag).Callbacks.Add(Handle, MoveTemp(Callback));
	return Handle;
}

void UGameplayEventBus::Unsubscribe(FGameplayTag EventTag, FDelegateHandle Handle)
{
	if (FNativeSubscribers* Subscribers = NativeSubscribersByTag.Find(EventTag))
	{
		Subscribers->Callbacks.Remove(Handle);
	}
}
