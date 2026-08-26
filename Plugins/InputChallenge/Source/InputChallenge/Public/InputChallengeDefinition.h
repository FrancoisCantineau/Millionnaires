#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "InputCoreTypes.h"
#include "GameplayTagContainer.h"
#include "InputChallengeDefinition.generated.h"

class UInputChallengeWidget;

UENUM(BlueprintType)
enum class EInputChallengeType : uint8
{
	Spam,
	Sequence,
	Hold,
};

/** One expected input, in both languages: the UInputAction (used only for the glyph/icon
 *  display, e.g. showing the right keyboard key), and the FGameplayTag it corresponds to
 *  (used to match events coming through IInputReceiverInterface, since that's all Tag-based).
 *  Set both by hand to the SAME pairing your routing plugin (e.g. ContextFramework) uses in
 *  its own mapping asset - InputChallenge never reads that asset directly, this is the only
 *  place the two need to agree. */
USTRUCT(BlueprintType)
struct FExpectedInputEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	TObjectPtr<UInputAction> Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	FGameplayTag Tag;
};

UCLASS(BlueprintType)
class INPUTCHALLENGE_API UInputChallengeDefinition : public UDataAsset
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	EInputChallengeType Type = EInputChallengeType::Spam;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	TArray<FExpectedInputEntry> ExpectedInputs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	int32 RequiredCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	float TimeLimit = 3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	float HoldDuration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge")
	bool bResetOnMistake = true;

	/** Optional. If set, this challenge globally slows down the whole world (bullet-time style)
	 *  while it's Running - restored to normal speed on success, failure, or abort.
	 *  NOTE: this is a WORLD-wide time dilation, not scoped to just this player's view - in
	 *  split-screen/multiplayer it would slow everyone down, not just whoever triggered it. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge|SlowMotion")
	bool bUseSlowMotion = false;

	/** Time dilation factor while the challenge is running - 1.0 = normal speed, 0.5 = half speed,
	 *  0.2 = very slow. Named as a factor (not "% slower") to match Unreal's own SetGlobalTimeDilation API directly. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge|SlowMotion", meta = (EditCondition = "bUseSlowMotion", ClampMin = "0.01", ClampMax = "1.0"))
	float SlowMotionScale = 0.5f;

	/** Optional. If set, this specific challenge shows this widget instead of the component's
	 *  default WidgetClass - e.g. a unique boss QTE getting a fancier skin than a regular
	 *  lockpick minigame. Whoever calls StartChallenge never needs to know or choose a widget
	 *  class either way - the visual style travels with the challenge's own data. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Challenge|UI")
	TSubclassOf<UInputChallengeWidget> WidgetClassOverride;
};

