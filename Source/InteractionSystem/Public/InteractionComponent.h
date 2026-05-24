#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractionComponent.generated.h"


/**
 * Component that handles player interaction with objects
 * Performs line traces to detect interactable objects
 */

DECLARE_DELEGATE_RetVal(bool, FCanInteractDelegate);



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERACTIONSYSTEM_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UInteractionComponent();

	/** Distance for interaction raycast */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDistance = 300.0f;

	/** Radius for sphere trace */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRadius = 50.0f;

	/** Update rate for checking interactable objects (in seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float CheckInterval = 0.1f;

	/** Show debug lines for interaction trace */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Debug")
	bool bShowDebugTrace = false;
	
	FCanInteractDelegate CanInteractDelegate;

	/** Perform interaction with focused object */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact();

	/** Get currently focused interactable actor */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetFocusedActor() const { return FocusedActor; }

	/** Get interaction text from focused actor */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	FText GetFocusedInteractionText() const;

	/** Check if there's an interactable object in focus */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool HasFocusedActor() const { return FocusedActor != nullptr; }


protected:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	/** Currently focused interactable actor */
	UPROPERTY()
	AActor* FocusedActor;

	/** Timer for periodic checks */
	float CheckTimer;

	/** Perform trace to find interactable objects */
	void CheckForInteractables();

	/** Get camera location and direction for trace */
	bool GetCameraViewPoint(FVector& OutLocation, FVector& OutDirection) const;
};