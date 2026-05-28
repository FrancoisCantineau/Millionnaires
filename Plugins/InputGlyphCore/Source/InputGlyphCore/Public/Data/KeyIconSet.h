#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputCoreTypes.h"
#include "KeyIconSet.generated.h"

/**
 * High-level glyph abstraction (UI-friendly, device-agnostic)
 */
UENUM(BlueprintType)
enum class EInputGlyphType : uint8
{
    Keyboard,
    Mouse,

    Gamepad_Button,
    Gamepad_Stick_Left,
    Gamepad_Stick_Right,
    Gamepad_DPad,
    Gamepad_Trigger_Left,
    Gamepad_Trigger_Right,

    Unknown
};

/**
 * DataAsset that maps inputs to UI icons
 */
UCLASS()
class INPUTGLYPHCORE_API UKeyIconSet : public UDataAsset
{
    GENERATED_BODY()

public:

    // Direct override per key (optional)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icons")
    TMap<FKey, TSoftObjectPtr<UTexture2D>> KeyIcons;

    // Glyph-based system (MAIN)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icons|Glyphs")
    TMap<EInputGlyphType, TSoftObjectPtr<UTexture2D>> GlyphIcons;

    // Fallback icon
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icons")
    TSoftObjectPtr<UTexture2D> FallbackIcon;

public:

    // --------------------------------------------------
    // KEY → GLYPH TYPE
    // --------------------------------------------------
    EInputGlyphType GetGlyphType(FKey Key) const
    {
        if (!Key.IsValid())
            return EInputGlyphType::Unknown;

        // -------------------------
        // GAMEPAD STICKS
        // -------------------------
        if (Key == EKeys::Gamepad_LeftX ||
            Key == EKeys::Gamepad_LeftY ||
            Key == EKeys::Gamepad_LeftThumbstick)
        {
            return EInputGlyphType::Gamepad_Stick_Left;
        }

        if (Key == EKeys::Gamepad_RightX ||
            Key == EKeys::Gamepad_RightY ||
            Key == EKeys::Gamepad_RightThumbstick)
        {
            return EInputGlyphType::Gamepad_Stick_Right;
        }

        // -------------------------
        // DPAD
        // -------------------------
        if (Key == EKeys::Gamepad_DPad_Up ||
            Key == EKeys::Gamepad_DPad_Down ||
            Key == EKeys::Gamepad_DPad_Left ||
            Key == EKeys::Gamepad_DPad_Right)
        {
            return EInputGlyphType::Gamepad_DPad;
        }

        // -------------------------
        // TRIGGERS
        // -------------------------
        if (Key == EKeys::Gamepad_LeftTriggerAxis)
        {
            return EInputGlyphType::Gamepad_Trigger_Left;
        }

        if (Key == EKeys::Gamepad_RightTriggerAxis)
        {
            return EInputGlyphType::Gamepad_Trigger_Right;
        }

        // -------------------------
        // FACE BUTTONS
        // -------------------------
        if (Key == EKeys::Gamepad_FaceButton_Bottom ||
            Key == EKeys::Gamepad_FaceButton_Top ||
            Key == EKeys::Gamepad_FaceButton_Left ||
            Key == EKeys::Gamepad_FaceButton_Right)
        {
            return EInputGlyphType::Gamepad_Button;
        }

        // -------------------------
        // MOUSE
        // -------------------------
        if (Key == EKeys::LeftMouseButton ||
            Key == EKeys::RightMouseButton ||
            Key == EKeys::MiddleMouseButton ||
            Key == EKeys::ThumbMouseButton ||
            Key == EKeys::ThumbMouseButton2)
        {
            return EInputGlyphType::Mouse;
        }

        // -------------------------
        // EVERYTHING ELSE = KEYBOARD
        // -------------------------
        return EInputGlyphType::Keyboard;
    }

    // --------------------------------------------------
    // MAIN RESOLVER (USED BY UI)
    // --------------------------------------------------
    UTexture2D* GetIconForKey(FKey Key) const
    {
        // 1. Direct override first
        if (const TSoftObjectPtr<UTexture2D>* Found = KeyIcons.Find(Key))
        {
            return Found->LoadSynchronous();
        }

        // 2. Glyph fallback
        const EInputGlyphType Type = GetGlyphType(Key);

        if (const TSoftObjectPtr<UTexture2D>* Glyph = GlyphIcons.Find(Type))
        {
            return Glyph->LoadSynchronous();
        }

        // 3. Final fallback
        return FallbackIcon.IsValid()
            ? FallbackIcon.LoadSynchronous()
            : nullptr;
    }
};