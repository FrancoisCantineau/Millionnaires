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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FContextHandle Handle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UContextDataAsset> Definition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FContextTransitionData TransitionData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> InteractionPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> ViewTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObject> InputReceiver = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> LookTarget = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> Source = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> SnapTarget = nullptr;
	
};