#pragma once

#include "CoreMinimal.h"
#include "Data/TraversalTypes.h"
#include "UObject/Interface.h"
#include "TraversalInterface.generated.h"

class UArrowComponent;
class ATraversalActor;

/**
 * Context passed to consumable effects
 */

UINTERFACE(MinimalAPI, Blueprintable)
class UTraversalInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 */
class TRAVERSALSYSTEM_API ITraversalInterface
{
    GENERATED_BODY()

public:
    
    virtual void TryStartTraversal(ATraversalActor* Target);
    virtual void OnTraversalNotify(ETraversalNotifyType EventType) {}
};
