#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "InputCoreTypes.h"
#include "InputChallengeDefinition.generated.h"

UENUM(BlueprintType)
enum class EInputChallengeType : uint8
{
	Spam,
	Sequence,
	Hold,
};

UCLASS(BlueprintType)
class INPUTCHALLENGE_API UInputChallengeDefinition : public UDataAsset
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	EInputChallengeType Type = EInputChallengeType::Spam;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	TArray<TObjectPtr<UInputAction>> ExpectedActions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	int32 RequiredCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	float TimeLimit = 3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	float HoldDuration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	bool bResetOnMistake = true;
};

