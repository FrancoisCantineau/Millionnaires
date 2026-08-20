#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayEventContext.generated.h"

/**
 * Context transported for each event published on the gameplay event bus.
 * A Sender/Instigator invalid is a normal use case to handle : context can survive to its emitter unload.
 */

USTRUCT(BlueprintType)
struct GAMEPLAYCORE_API FEventContext
{
	GENERATED_BODY()

public :

	/** The specific tag for This event (Ex : "Event.Door.Closed"). Used as a routing key for the GameplayEventSubsystem */
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag EventTag;
	
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> Sender;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> Instigator;

	UPROPERTY(BlueprintReadWrite)
	FVector Location;

	UPROPERTY(BlueprintReadOnly, Category = "Event Context")
	double WorldTimeSeconds = 0.0;

	/** Free additionnal tags. */
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Context")
	FGameplayTagContainer AdditionalTags;

	FEventContext() = default;

	static FEventContext Make(FGameplayTag InEventTag, AActor* InSender, AActor* InInstigator = nullptr)
	{
		FEventContext Ctx;
		Ctx.EventTag = InEventTag;
		Ctx.Sender = InSender;
		Ctx.Instigator = InInstigator ? InInstigator : InSender;
		Ctx.Location = InSender ? InSender->GetActorLocation() : FVector::ZeroVector;
		Ctx.WorldTimeSeconds = (InSender && InSender->GetWorld()) ? InSender->GetWorld()->GetTimeSeconds() : 0.0;
		return Ctx;
	}
	
};
