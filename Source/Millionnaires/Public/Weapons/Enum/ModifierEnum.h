/* 
* Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Enum to handle different modifier types. Such as the addition and multiplication of buffs
 */

#pragma once

UENUM(BlueprintType)
enum class EModifierType : uint8
{
    Additive UMETA(DisplayName = "Additive (+)"),
    Multiplicative UMETA(DisplayName = "Multiplicative (x)")
};