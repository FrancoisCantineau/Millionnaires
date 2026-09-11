// DisappearComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DisappearComponent.generated.h"

class USoundBase;

UENUM(BlueprintType)
enum class EDisappearState : uint8
{
	Appeared,
	Disappeared
};

/**
 * Shared "how to disappear" implementation — add to ANY enemy that needs this capability.
 * Unlike an interface, this actually shares the mechanics (sound, hide, disable collision)
 * instead of making every enemy class reimplement the same thing. BTTask_SetDisappearState (and
 * anything else that needs to trigger a disappearance — a cutscene, a trigger volume, a scripted
 * event) just calls SetDisappearState() on this component.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MILLIONNAIRES_API UDisappearComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDisappearComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disappear")
	TObjectPtr<USoundBase> DisappearSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disappear")
	TObjectPtr<USoundBase> AppearSound;

	UFUNCTION(BlueprintCallable, Category = "Disappear")
	void SetDisappearState(EDisappearState NewState);

	UFUNCTION(BlueprintCallable, Category = "Disappear")
	void Disappear();

	UFUNCTION(BlueprintCallable, Category = "Disappear")
	void Appear();

	UFUNCTION(BlueprintPure, Category = "Disappear")
	bool IsDisappeared() const { return bIsDisappeared; }

private:
	bool bIsDisappeared = false;
};