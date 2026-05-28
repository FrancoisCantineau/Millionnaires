#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CharacterInterface.generated.h"

/**
 * Context passed to consumable effects
 */

UINTERFACE(MinimalAPI, Blueprintable)
class UCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 */
class GAMEINTERFACES_API ICharacterInterface
{
	GENERATED_BODY()

public:


    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void AddStateTag(FGameplayTag StateTag);
	
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void RemoveStateTag(FGameplayTag StateTag) ;
	
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	bool HasStateTag(FGameplayTag StateTag);
   
};
