// DialogueSpeaker.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DialogueSpeaker.generated.h"

class UTexture2D;

/**
 * A reusable character identity for dialogue lines — author a character's name/portrait/color
 * once, reference it from every line instead of retyping the name across dozens of lines.
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEM_API UDialogueSpeaker : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TObjectPtr<UTexture2D> Portrait;

	/** For subtitle UI theming (name color, etc.) — purely cosmetic data, plugin doesn't use it itself. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FLinearColor NameColor = FLinearColor::White;
};
