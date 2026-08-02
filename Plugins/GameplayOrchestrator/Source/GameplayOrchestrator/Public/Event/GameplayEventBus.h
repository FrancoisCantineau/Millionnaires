#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/GameplayEventContext.h"
#include "GameplayEventBus.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayEventReceived, const FEventContext&, Context);

/**
 * Bus pub/sub per Gameplay Tag. Shares instant events (ex : "Event.Door.Closed").
 * Doesn't stock persistant state and doesn't know anything about gameplay logic.
 */
UCLASS()
class GAMEPLAYORCHESTRATOR_API UGameplayEventBus : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay Orchestrator|Event Bus")
	void Broadcast(FGameplayTag EventTag, const FEventContext& Context);

	/** Helper : build and publish a FEventContext in a call. */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Orchestrator|Event Bus", meta = (DefaultToSelf = "Sender"))
	void PublishEvent(FGameplayTag EventTag, AActor* Sender, AActor* Instigator = nullptr);

	FDelegateHandle SubscribeNative(FGameplayTag EventTag, TFunction<void(const FEventContext&)> Callback);
	void Unsubscribe(FGameplayTag EventTag, FDelegateHandle Handle);

	UPROPERTY(BlueprintAssignable, Category = "Gameplay Orchestrator|Event Bus")
	FOnGameplayEventReceived OnEventReceived;

private:
	struct FNativeSubscribers
	{
		TMap<FDelegateHandle, TFunction<void(const FEventContext&)>> Callbacks;
	};

	TMap<FGameplayTag, FNativeSubscribers> NativeSubscribersByTag;
	TMap<FGameplayTag, FOnGameplayEventReceived> BlueprintSubscribersByTag;
};