#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "QuestObjectiveEntryWidget.generated.h"

class UQuestObjective;
class UTextBlock;
class UProgressBar;

/**
 * Widget representing a single quest objective entry in the quest log.
 * - Displays objective description, progress, and timer if applicable.
 */
UCLASS()
class QUESTSYSTEM_API UQuestObjectiveEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	void InitializeObjective(UQuestObjective* InObjective);
	void UpdateProgress();
	void UpdateTimerDisplay(float RemainingTime);
	
	/* Function to play completion animation */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void PlayCompletionAnimation();

protected:
	
	/* Objective description */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ObjectiveDescriptionText;

	/* Progress display */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ProgressText;

	/* Progress bar */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* ProgressBar;

	/* Timer display */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* TimerText;

	/* Timer progress bar */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UProgressBar* TimerProgressBar;
	
	/* Completion animation */
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* CompletionAnim;

private:
	
	/* The quest objective this widget represents */
	UPROPERTY()
	UQuestObjective* Objective;
};