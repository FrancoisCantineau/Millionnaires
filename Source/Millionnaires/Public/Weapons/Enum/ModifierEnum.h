#pragma once

UENUM(BlueprintType)
enum class EModifierType : uint8
{
    Additive UMETA(DisplayName = "Additive (+)"),
    Multiplicative UMETA(DisplayName = "Multiplicative (x)")
};