// Fill out your copyright notice in the Description page of Project Settings.
	

#include "ContextRequestResolver.h"

#include "ContextComponent.h"
#include "Data/ContextDataAsset.h"
#include "Data/ContextStructData.h"
#include "Event/GameplayEventBus.h"
#include "Core/GameplayEventContext.h"
#include "Tags/CoreGameplayTags.h"


UContextRequestResolver::UContextRequestResolver()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UContextRequestResolver::BeginPlay()
{
	Super::BeginPlay();

	ContextComponent = GetOwner() ? GetOwner()->FindComponentByClass<UContextComponent>() : nullptr;

	if (const UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			Bus = GameInstance->GetSubsystem<UGameplayEventBus>();
		}
	}

	if (!Bus)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ContextRequestResolver] GameplayEventBus not found, context requests will never be resolved."));
		return;
	}

	if (!ContextComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ContextRequestResolver] No UContextComponent found on owner '%s'."), *GetNameSafe(GetOwner()));
		return;
	}

	PushSubscriptionHandle = Bus->SubscribeNative(
		TAG_Request_Context_Push,
		[this](const FEventContext& Ctx) { HandlePushRequest(Ctx); });

	PopSubscriptionHandle = Bus->SubscribeNative(
		TAG_Request_Context_Pop,
		[this](const FEventContext& Ctx) { HandlePopRequest(Ctx); });
}

void UContextRequestResolver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Bus)
	{
		Bus->Unsubscribe(TAG_Request_Context_Push, PushSubscriptionHandle);
		Bus->Unsubscribe(TAG_Request_Context_Pop, PopSubscriptionHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void UContextRequestResolver::HandlePushRequest(const FEventContext& Ctx)
{
	if (!ContextComponent) return;

	if (Ctx.AdditionalTags.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ContextRequestResolver] Push request with no context tag in AdditionalTags, ignored."));
		return;
	}

	const FGameplayTag ContextId = Ctx.AdditionalTags.GetGameplayTagArray()[0];

	const TObjectPtr<UContextDataAsset>* FoundAsset = ContextRegistry.Find(ContextId);
	if (!FoundAsset || !*FoundAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ContextRequestResolver] No context registered for tag '%s'."), *ContextId.ToString());
		return;
	}

	FActiveContext ActiveContext;
	ActiveContext.Definition = *FoundAsset;
	ActiveContext.InputReceiver = Ctx.Payload.Get();
	ActiveContext.Source = Ctx.Sender.Get();

	const FContextHandle Handle = ContextComponent->AddContext(ActiveContext);
	if (Handle.IsValid())
	{
		ActiveHandlesByTag.Add(ContextId, Handle);
	}
}

void UContextRequestResolver::HandlePopRequest(const FEventContext& Ctx)
{
	if (!ContextComponent) return;

	if (Ctx.AdditionalTags.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ContextRequestResolver] Pop request with no context tag in AdditionalTags, ignored."));
		return;
	}

	const FGameplayTag ContextId = Ctx.AdditionalTags.GetGameplayTagArray()[0];

	if (const FContextHandle* Handle = ActiveHandlesByTag.Find(ContextId))
	{
		ContextComponent->RemoveContext(*Handle);
		ActiveHandlesByTag.Remove(ContextId);
	}
}
