#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "RepairProgressWidget.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * Widget to display repair progress of equipment
 * - Shows a progress bar and equipment name
 */
UCLASS()
class QUESTSYSTEM_API URepairProgressWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Update the progress bar value */
	UFUNCTION(BlueprintCallable, Category = "Repair")
	void SetProgress(float Progress);

	/** Set equipment name text */
	UFUNCTION(BlueprintCallable, Category = "Repair")
	void SetEquipmentName(const FText& Name);

	/** Get current progress */
	UFUNCTION(BlueprintPure, Category = "Repair")
	float GetProgress() const { return CurrentProgress; }

protected:

	virtual void NativeConstruct() override;

	/** Progress bar widget */
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar;

	/** Equipment name text */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* EquipmentNameText;

	/** Progress percentage text */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ProgressText;

	/** Current progress value */
	UPROPERTY(BlueprintReadOnly, Category = "Repair")
	float CurrentProgress;
};