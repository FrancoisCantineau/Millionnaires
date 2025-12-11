#pragma once

UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	Simple UMETA(DisplayName = "Simple"),
	Explosive UMETA(DisplayName = "Explosive"),
};