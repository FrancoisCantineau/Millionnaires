// WeightedRandomLibrary.h
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeightedRandomLibrary.generated.h"

UCLASS()
class MILLIONNAIRES_API UWeightedRandomLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Picks a random index in [0, Weights.Num()-1], weighted by each entry's value (higher =
	 *  more likely). Weights are relative — they don't need to sum to 1 or 100 (e.g. 5/3/2 works
	 *  exactly like 50/30/20). Returns -1 if Weights is empty or every weight is <= 0. */
	UFUNCTION(BlueprintCallable, Category = "Random")
	static int32 WeightedRandomIndex(const TArray<float>& Weights);
};