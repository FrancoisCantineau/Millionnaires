#pragma once


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