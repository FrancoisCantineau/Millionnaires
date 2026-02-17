#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"

#include "RepairableEquipmentActor.generated.h"

class URepairObjective;
class URepairProgressWidget;
class UWidgetComponent;
class UStaticMeshComponent;

/**
 * Actor representing repairable equipment for repair objectives
 * 
 * - Allows players to interact and repair over time
 * - Displays progress via a widget
 * - Plays sounds and VFX during repair process
 */
UCLASS()
class QUESTSYSTEM_API ARepairableEquipmentActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:

	ARepairableEquipmentActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	void SetTickOptimization(bool bEnabled);

	/** IInteractionInterface implementation */
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionDisplayName_Implementation() const override;

	/** Set the objective that owns this repair point */
	void SetOwningObjective(URepairObjective* Objective);

	/** Enable repair functionality */
	void EnableRepair();

	/** Disable repair functionality */
	void DisableRepair();

	/** Check if repair is enabled */
	UFUNCTION(BlueprintPure, Category = "Repair")
	bool IsRepairEnabled() const { return bRepairEnabled; }

	/** Check if repair is completed */
	UFUNCTION(BlueprintPure, Category = "Repair")
	bool IsRepaired() const { return bIsRepaired; }

	/** Get current repair progress */
	UFUNCTION(BlueprintPure, Category = "Repair")
	float GetRepairProgress() const;

protected:

	/** Visual mesh for the equipment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* EquipmentMesh;

	/** Widget component for progress bar */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* ProgressWidgetComponent;

	/** Progress widget reference */
	UPROPERTY()
	URepairProgressWidget* ProgressWidget;

	/** Time required to complete repair */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Repair")
	float RepairDuration;

	/** Equipment display name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Repair")
	FText EquipmentName;

	/** Current repair progress time */
	UPROPERTY(BlueprintReadOnly, Category = "Repair")
	float CurrentRepairTime;

	/** Is repair currently in progress */
	UPROPERTY(BlueprintReadOnly, Category = "Repair")
	bool bIsRepairing;

	/** Is repair enabled */
	UPROPERTY(BlueprintReadOnly, Category = "Repair")
	bool bRepairEnabled;

	/** Is repair completed */
	UPROPERTY(BlueprintReadOnly, Category = "Repair")
	bool bIsRepaired;

	/** Reference to owning objective */
	UPROPERTY()
	URepairObjective* OwningObjective;

	/** Actor currently repairing this equipment */
	UPROPERTY()
	AActor* CurrentInteractor;

	/** Widget class for progress bar */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<URepairProgressWidget> ProgressWidgetClass;

	/** Sound: Repair loop */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* RepairLoopSound;

	/** Sound: Repair completed */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* RepairCompletedSound;

	/** Sound: Repair cancelled */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* RepairCancelledSound;

	/** VFX: Repair particles */
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UParticleSystem* RepairParticles;

	/** VFX: Completion flash */
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UParticleSystem* CompletionFlash;

	/** Audio component for repair loop */
	UPROPERTY()
	UAudioComponent* RepairAudioComponent;

	/** Particle component for repair VFX */
	UPROPERTY()
	UParticleSystemComponent* RepairParticleComponent;

	/** Timer handle for input check optimization */
	FTimerHandle InputCheckTimer;

	/** Cached last input state */
	bool bWasInteracting;

private:

	/** Start repair process */
	void StartRepair(AActor* Interactor);

	/** Update repair progress */
	void UpdateRepair(float DeltaTime);

	/** Cancel repair process */
	void CancelRepair();

	/** Complete repair process */
	void CompleteRepair();

	/** Check if player is still holding E and in range */
	bool IsPlayerStillInteracting() const;

	/** Update progress widget visibility and value */
	void UpdateProgressWidget();
	
	/** Check input state */
	void CheckInputState();

	/** Play repair sounds and VFX */
	void PlayRepairEffects();

	/** Stop repair sounds and VFX */
	void StopRepairEffects();

	/** Play completion effects */
	void PlayCompletionEffects();
};