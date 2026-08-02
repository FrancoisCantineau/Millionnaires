#pragma once

#include "CoreMinimal.h"
#include "GameplayActionBase.h"
#include "GameplayTagContainer.h"
#include "GameplayAction_SetState.generated.h"

/** Writes a state in the worldStateSubsystem. Don't ever reference an actor directly */
UCLASS(meta = (DisplayName = "Set State"))
class GAMEPLAYORCHESTRATOR_API UGameplayAction_SetState : public UGameplayActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set State")
	FGameplayTag StateKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set State")
	FGameplayTag StateValue;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set State")
	EWorldStatePersistence Persistence = EWorldStatePersistence::SaveGame;*/

	virtual void Execute_Implementation(const FEventContext& Context) override;

#if WITH_EDITOR
	virtual FString GetEditorSummary() const override;
#endif
};
