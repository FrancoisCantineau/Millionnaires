#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayEventContext.generated.h"

class UGameplayActionBase;

USTRUCT(BlueprintType)
struct FEventContext
{
	GENERATED_BODY()

public :

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag EventTag;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Sender;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Instigator;

	UPROPERTY(BlueprintReadWrite)
	FVector Location;
	
};

USTRUCT(BlueprintType)
struct FGameplaySequenceStep
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere)
	float DelayBeforeStep = 0.f;
    
	UPROPERTY(EditAnywhere, Instanced)
	TArray<TObjectPtr<UGameplayActionBase>> ParallelActions;
};

UCLASS(BlueprintType)
class UGameplaySequence : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FName SequenceID;

	UPROPERTY(EditAnywhere)
	TArray<FGameplaySequenceStep> Steps;
};