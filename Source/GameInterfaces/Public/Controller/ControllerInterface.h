#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ControllerInterface.generated.h"

enum class EPlayerAction : uint8;

UENUM(BlueprintType)
enum class EPlayerMode : uint8
{
	Gameplay UMETA(DisplayName = "Gameplay"),
	Inspect UMETA(DisplayName = "Inspect"),
	Hide,
	Terminal,
	Cinematic,
	InputChallenge  UMETA(DisplayName = "Input Challenge"),
	Traversal,
};
/**
 * Context passed to consumable effects
 */

UENUM(BlueprintType)
enum class EPlayerAction : uint8
{
	Move,
	Look,
	Jump,
	Interact,
	UseItem,
	ToggleFlashlight,
	ExitMode
};

UINTERFACE(MinimalAPI, Blueprintable)
class UControllerInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMEINTERFACES_API IControllerInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetPlayerMode(EPlayerMode NewMode, AActor* ContextActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanPerform(EPlayerAction Action) const;
};
