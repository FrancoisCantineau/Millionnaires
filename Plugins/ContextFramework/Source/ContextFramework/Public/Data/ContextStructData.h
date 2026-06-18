#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ContextStructData.generated.h"

class UContextDataAsset;

USTRUCT(BlueprintType)
struct CONTEXTFRAMEWORK_API FActiveContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UContextDataAsset> Definition;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USceneComponent> InteractionPoint;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> ViewTarget = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> InputReceiver = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USceneComponent> LookTarget = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> Source = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USceneComponent> SnapTarget = nullptr;

	bool operator==(const FActiveContext& Other) const
	{
		return Definition == Other.Definition
		&& InteractionPoint == Other.InteractionPoint
	&& ViewTarget == Other.ViewTarget
	&& InputReceiver == Other.InputReceiver
	&& Source == Other.Source
	&& SnapTarget == Other.SnapTarget;
		
	}
};