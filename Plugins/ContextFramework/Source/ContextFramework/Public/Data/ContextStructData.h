#pragma once

#include "CoreMinimal.h"
#include "ContextStructData.generated.h"

class UContextDataAsset;

USTRUCT(BlueprintType)
struct CONTEXTFRAMEWORK_API FActiveContext
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UContextDataAsset> Definition;

	UPROPERTY()
	TObjectPtr<AActor> ViewTarget = nullptr;

	bool operator==(const FActiveContext& Other) const
	{
		return Definition == Other.Definition
			&& ViewTarget == Other.ViewTarget;
		
	}
};