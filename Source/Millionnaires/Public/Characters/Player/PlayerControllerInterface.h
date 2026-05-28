#pragma once


#include "CoreMinimal.h"
#include "Controller/ControllerInterface.h"
#include "UObject/Interface.h"
#include "PlayerControllerInterface.generated.h"

enum class EPlayerAction : uint8;

UINTERFACE(MinimalAPI)
class UPlayerControllerInterface : public UInterface
{
    GENERATED_BODY()
};

class MILLIONNAIRES_API IPlayerControllerInterface
{
    GENERATED_BODY()

public:
    virtual void SetPlayerMode(EPlayerMode NewMode, AActor* ContextActor = nullptr) = 0;
    virtual bool CanPerform(EPlayerAction Action) const = 0;
};