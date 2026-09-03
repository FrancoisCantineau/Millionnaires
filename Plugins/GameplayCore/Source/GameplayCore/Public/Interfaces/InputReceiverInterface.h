#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputActionValue.h"
#include "InputTriggers.h"
#include "GameplayTagContainer.h"
#include "InputReceiverInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInputReceiverInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Shared contract, lives in GameplayCore rather than in any single routing plugin (like
 * ContextFramework) on purpose: a router plugin CALLS this interface without knowing who
 * implements it, and any receiver plugin (InputChallenge, TraversalSystem, ...) IMPLEMENTS it
 * without knowing which router calls it. Neither ever depends on the other directly - both
 * only depend on this tiny, stable interface.
 */
class GAMEPLAYCORE_API IInputReceiverInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandleInput(FGameplayTag Tag, const FInputActionValue& Value, ETriggerEvent TriggerEvent);
};
