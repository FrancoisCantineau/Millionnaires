#pragma once

#include "CoreMinimal.h"
#include "EffectInterface.generated.h"


UINTERFACE(Blueprintable)
class UEffectInterface : public UInterface
{
	GENERATED_BODY()
};

class IEffectInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SanityEffect();
};