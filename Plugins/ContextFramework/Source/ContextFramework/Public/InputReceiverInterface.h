#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h" 
#include "InputReceiverInterface.generated.h"


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

	UFUNCTION(BlueprintNativeEvent)
    void HandleInput(FGameplayTag Tag, const FInputActionValue& Value);
	
};