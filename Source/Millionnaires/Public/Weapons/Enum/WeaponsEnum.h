#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Melee UMETA(DisplayName = "Melee"),
    Ranged UMETA(DisplayName = "Ranged")
};