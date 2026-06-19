#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ContextStructData.generated.h"

USTRUCT(BlueprintType)
struct FContextHandle
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid Guid;

	bool IsValid() const
	{
		return Guid.IsValid();
	}

	bool operator==(const FContextHandle& Other) const
	{
		return Guid == Other.Guid;
	}
	void GenerateNewGuid()
	{
		Guid = FGuid::NewGuid();
	}
};

USTRUCT(BlueprintType)
struct FContextTransitionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> EnterMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> ExitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> SnapTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> LookTarget;
};

class UContextDataAsset;

USTRUCT(BlueprintType)
struct CONTEXTFRAMEWORK_API FActiveContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FContextHandle Handle;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UContextDataAsset> Definition;

	UPROPERTY(BlueprintReadOnly)
	FContextTransitionData TransitionData;

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
	
};