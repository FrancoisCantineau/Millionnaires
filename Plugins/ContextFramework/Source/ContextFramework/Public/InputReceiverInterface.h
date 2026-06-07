#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputReceiverInterface.generated.h"

struct FGameplayTag;

UINTERFACE(MinimalAPI, Blueprintable)
class UInputReceiverInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for interactable objects
 */
class CONTEXTFRAMEWORK_API IInputReceiverInterface
{
	GENERATED_BODY()

public:

	virtual void HandleInput(FGameplayTag Tag) = 0;
	
};