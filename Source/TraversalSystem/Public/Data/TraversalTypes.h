#pragma once

#include "CoreMinimal.h"
#include "TraversalTypes.generated.h"

class UAnimMontage;

UENUM(BlueprintType)
enum class ETraversalType : uint8
{
	None,
	Ladder,
	Duct,
	Ledge,
	NarrowPassage
};

UENUM(BlueprintType)
enum class ETraversalHand : uint8
{
	Right = 0,
	Left  = 1
};

UENUM(BlueprintType)
enum class ETraversalNotifyType : uint8
{
	None,
	StepLeft,
	StepRight,
	Enter,
	Exit
};

UENUM(BlueprintType)
enum class ETraversalState : uint8
{
	None,
	Approaching,
	Traversing,
	Exiting
};

USTRUCT(BlueprintType)
struct FTraversalAnimSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) UAnimMontage* MoveForwardRight   = nullptr;
	UPROPERTY(EditAnywhere) UAnimMontage* MoveForwardLeft    = nullptr;
	UPROPERTY(EditAnywhere) UAnimMontage* MoveBackwardRight = nullptr;
	UPROPERTY(EditAnywhere) UAnimMontage* MoveBackwardLeft  = nullptr;
	UPROPERTY(EditAnywhere) UAnimMontage* EnterStartRight      = nullptr;
	UPROPERTY(EditAnywhere) UAnimMontage* EnterStartLeft       = nullptr;
	UPROPERTY(EditAnywhere) UAnimMontage* EnterEndRight         = nullptr;
	UPROPERTY(EditAnywhere) UAnimMontage* EnterEndLeft         = nullptr;
	UPROPERTY(EditAnywhere) UAnimMontage* ExitEndRight       = nullptr;
	UPROPERTY(EditAnywhere) UAnimMontage* ExitEndLeft       = nullptr;
	UPROPERTY(EditAnywhere) UAnimMontage* ExitStartRight    = nullptr;
	UPROPERTY(EditAnywhere) UAnimMontage* ExitStartLeft    = nullptr;
};