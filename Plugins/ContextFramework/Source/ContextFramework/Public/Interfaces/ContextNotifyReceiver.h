#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h" 
#include "ContextNotifyReceiver.generated.h"


UINTERFACE(MinimalAPI, Blueprintable)
class UContextNotifyReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for interactable objects
 */
class CONTEXTFRAMEWORK_API IContextNotifyReceiver
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void OnContextNotify(FGameplayTag NotifyTag);
	
};