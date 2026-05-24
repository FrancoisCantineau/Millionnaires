
#pragma once

UENUM(BlueprintType)
enum class EPlayerMode : uint8
{
	Gameplay UMETA(DisplayName = "Gameplay"),
	Inspect UMETA(DisplayName = "Inspect"),
	Hide,
	Terminal,
	Cinematic,
	InputChallenge  UMETA(DisplayName = "Input Challenge"),
};