// WeightedRandomLibrary.cpp
#include "FunctionLibrary/WeightedRandomLibrary.h"

int32 UWeightedRandomLibrary::WeightedRandomIndex(const TArray<float>& Weights)
{
	float Total = 0.f;
	for (const float Weight : Weights)
	{
		Total += FMath::Max(Weight, 0.f);
	}


	if (Total <= 0.f)
	{
		return -1;
	}

	float Roll = FMath::FRandRange(0.f, Total);
	for (int32 i = 0; i < Weights.Num(); ++i)
	{
		const float Weight = FMath::Max(Weights[i], 0.f);
		if (Roll < Weight)
		{
			return i;
		}
		Roll -= Weight;
	}

	// Floating point edge case (Roll landed exactly on Total) — last entry is the safe fallback.
	return Weights.Num() - 1;
}