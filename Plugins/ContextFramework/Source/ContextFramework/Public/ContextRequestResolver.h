// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Data/ContextStructData.h"
#include "ContextRequestResolver.generated.h"

class UContextComponent;
class UContextDataAsset;
class UGameplayEventBus;
struct FEventContext;

/**
 * The only component in ContextFramework allowed to call UContextComponent::AddContext /
 * RemoveContext in response to an external request. Listens to Request.Context.Push and
 * Request.Context.Pop on the GameplayEventBus and resolves the tag carried in
 * FEventContext::AdditionalTags against a designer-filled registry.
 *
 * ContextFramework never includes headers from any other gameplay plugin (InputChallenge,
 * Ladder, Quest, etc.) - this is the single door through which those plugins can ask for a
 * context to be pushed or popped, entirely decoupled via GameplayCore's event bus.
 *
 * Place this on the same actor as UContextComponent (usually the Pawn or PlayerController).
 */
UCLASS(ClassGroup=(ContextFramework), meta=(BlueprintSpawnableComponent))
class CONTEXTFRAMEWORK_API UContextRequestResolver : public UActorComponent
{
	GENERATED_BODY()

public:
	UContextRequestResolver();

	/** Filled in the editor: Tag ("Context.*") -> asset to push when that tag is requested. */
	UPROPERTY(EditDefaultsOnly, Category = "ContextFramework|Requests")
	TMap<FGameplayTag, TObjectPtr<UContextDataAsset>> ContextRegistry;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void HandlePushRequest(const FEventContext& Ctx);
	void HandlePopRequest(const FEventContext& Ctx);

	UPROPERTY()
	TObjectPtr<UContextComponent> ContextComponent;

	UPROPERTY()
	TObjectPtr<UGameplayEventBus> Bus;

	/** Tracks which FContextHandle is currently active for a given "Context.*" tag, so a Pop
	 *  request only removes the exact context that was pushed for that tag. */
	TMap<FGameplayTag, FContextHandle> ActiveHandlesByTag;

	FDelegateHandle PushSubscriptionHandle;
	FDelegateHandle PopSubscriptionHandle;
};
