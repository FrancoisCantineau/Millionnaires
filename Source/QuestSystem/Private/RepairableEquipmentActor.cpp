#include "RepairableEquipmentActor.h"

#include "RepairObjective.h"
#include "UI/RepairProgressWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

#pragma region Initialization

/*
 * Constructor
 * - Sets up components
 * - Initializes default values
 */
ARepairableEquipmentActor::ARepairableEquipmentActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	EquipmentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquipmentMesh"));
	RootComponent = EquipmentMesh;

	ProgressWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProgressWidget"));
	ProgressWidgetComponent->SetupAttachment(RootComponent);
	ProgressWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	ProgressWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	ProgressWidgetComponent->SetDrawSize(FVector2D(200.0f, 50.0f));
	ProgressWidgetComponent->SetVisibility(false);

	RepairDuration = 5.0f;
	EquipmentName = FText::FromString("Equipment");
	CurrentRepairTime = 0.0f;
	bIsRepairing = false;
	bRepairEnabled = false;
	bIsRepaired = false;
	OwningObjective = nullptr;
	CurrentInteractor = nullptr;
	bWasInteracting = false;
}

/*
 * BeginPlay
 * - Initializes the progress widget
 * - Sets initial progress to 0
 * - Sets equipment name on the widget
 */
void ARepairableEquipmentActor::BeginPlay()
{
	Super::BeginPlay();

	if (ProgressWidgetComponent && ProgressWidgetClass)
	{
		ProgressWidgetComponent->SetWidgetClass(ProgressWidgetClass);
		ProgressWidget = Cast<URepairProgressWidget>(ProgressWidgetComponent->GetWidget());
		
		if (ProgressWidget)
		{
			ProgressWidget->SetProgress(0.0f);
			ProgressWidget->SetEquipmentName(EquipmentName);
		}
	}
}

/*
 * Tick
 * - Updates repair progress if repairing
 */
void ARepairableEquipmentActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRepairing)
	{
		UpdateRepair(DeltaTime);
	}
}

/*
 * SetTickOptimization
 * - Enables or disables actor tick based on repair state
 */
void ARepairableEquipmentActor::SetTickOptimization(bool bEnabled)
{
	SetActorTickEnabled(bEnabled);
}

#pragma endregion

#pragma region Interaction Interface

/*
 * Interact
 * - Starts repair if enabled and not already repaired
 * - Ignores interaction if repair is disabled or already completed
 * 
 * @param Interactor The actor interacting with this equipment
 */
void ARepairableEquipmentActor::Interact_Implementation(AActor* Interactor)
{
	if (!bRepairEnabled || bIsRepaired)
	{
		return;
	}

	if (!bIsRepairing)
	{
		StartRepair(Interactor);
	}
}

/*
 * GetInteractionDisplayName
 * - Returns appropriate interaction text based on repair state
 * 
 * @return FText The interaction display name
 */
FText ARepairableEquipmentActor::GetInteractionDisplayName_Implementation() const
{
	if (!bRepairEnabled)
	{
		return FText::FromString("Not available");
	}

	if (bIsRepaired)
	{
		return FText::FromString("Already repaired");
	}

	return FText::Format(FText::FromString("Repair {0}"),EquipmentName);
}

#pragma endregion

#pragma region Repair Logic

/*
 * StartRepair
 * - Initiates the repair process
 * - Sets repairing state and resets progress
 * - Enables tick for repair updates
 * - Starts input check timer for optimization
 * 
 * @param Interactor The actor starting the repair
 */
void ARepairableEquipmentActor::StartRepair(AActor* Interactor)
{
	if (bIsRepairing || bIsRepaired)
	{
		return;
	}

	CurrentInteractor = Interactor;
	bIsRepairing = true;
	CurrentRepairTime = 0.0f;
	bWasInteracting = true;

	SetTickOptimization(true);

	GetWorld()->GetTimerManager().SetTimer(
		InputCheckTimer,
		this,
		&ARepairableEquipmentActor::CheckInputState,
		0.1f,
		true
	);

	UpdateProgressWidget();
	PlayRepairEffects();
}

/*
 * UpdateRepair
 * - Updates repair progress based on DeltaTime
 * - Completes repair if duration is reached
 * 
 * @param DeltaTime Time elapsed since last tick
 */
void ARepairableEquipmentActor::UpdateRepair(float DeltaTime)
{
	CurrentRepairTime += DeltaTime;
	UpdateProgressWidget();

	if (CurrentRepairTime >= RepairDuration)
	{
		CompleteRepair();
	}
}

/*
 * CancelRepair
 * - Cancels the ongoing repair process
 * - Resets repairing state and progress
 * - Disables tick for optimization
 * - Clears input check timer
 * - Stops repair effects
 * - Plays cancellation sound
 */
void ARepairableEquipmentActor::CancelRepair()
{
	if (!bIsRepairing)
	{
		return;
	}

	bIsRepairing = false;
	CurrentRepairTime = 0.0f;
	CurrentInteractor = nullptr;

	SetTickOptimization(false);

	GetWorld()->GetTimerManager().ClearTimer(InputCheckTimer);

	UpdateProgressWidget();
	StopRepairEffects();

	if (RepairCancelledSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, RepairCancelledSound, GetActorLocation());
	}
}

/*
 * CompleteRepair
 * - Finalizes the repair process
 * - Sets repaired state and resets repairing state
 * - Disables tick for optimization
 * - Clears input check timer
 * - Stops repair effects
 * - Plays completion effects
 * - Notifies owning objective of completion
 */
void ARepairableEquipmentActor::CompleteRepair()
{
	if (bIsRepaired)
	{
		return;
	}

	bIsRepairing = false;
	bIsRepaired = true;
	CurrentInteractor = nullptr;

	SetTickOptimization(false);

	GetWorld()->GetTimerManager().ClearTimer(InputCheckTimer);

	UpdateProgressWidget();
	StopRepairEffects();
	PlayCompletionEffects();
	
	if (OwningObjective)
	{
		OwningObjective->OnRepairPointCompleted(this);
	}
}

/*
 * CheckInputState
 * - Monitors player input and distance during repair
 * - Cancels repair if player moves out of range or releases interaction key
 */
void ARepairableEquipmentActor::CheckInputState()
{
	if (!CurrentInteractor)
	{
		CancelRepair();
		return;
	}

	float Distance = FVector::Dist(GetActorLocation(), CurrentInteractor->GetActorLocation());
	const float MaxDistance = 400.0f;

	if (Distance > MaxDistance)
	{
		CancelRepair();
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Cast<APawn>(CurrentInteractor)->GetController());
	if (!PC)
	{
		CancelRepair();
		return;
	}

	bool bIsInteracting = PC->IsInputKeyDown(EKeys::E);
	if (!bIsInteracting && bWasInteracting)
	{
		CancelRepair();
	}

	bWasInteracting = bIsInteracting;
}

/*
 * IsPlayerStillInteracting
 * - Checks if the player is still interacting with the equipment
 * 
 * @return bool True if still interacting, false otherwise
 */
bool ARepairableEquipmentActor::IsPlayerStillInteracting() const
{
	return bIsRepairing;
}

/*
 * UpdateProgressWidget
 * - Updates the visibility and progress value of the progress widget
 */
void ARepairableEquipmentActor::UpdateProgressWidget()
{
	if (!ProgressWidget || !ProgressWidgetComponent)
	{
		return;
	}

	if (bIsRepairing)
	{
		ProgressWidgetComponent->SetVisibility(true);
		ProgressWidget->SetProgress(GetRepairProgress());
	}
	else
	{
		ProgressWidgetComponent->SetVisibility(false);
		ProgressWidget->SetProgress(0.0f);
	}
}

#pragma endregion

#pragma region Audio and VFX

/*
 * PlayRepairEffects
 * - Starts repair loop sound and particle effects
 */
void ARepairableEquipmentActor::PlayRepairEffects()
{
	if (RepairLoopSound)
	{
		RepairAudioComponent = UGameplayStatics::SpawnSoundAttached(
			RepairLoopSound,
			RootComponent,
			NAME_None,
			FVector::ZeroVector,
			EAttachLocation::SnapToTarget,
			false,
			1.0f,
			1.0f,
			0.0f,
			nullptr,
			nullptr,
			true
		);
	}

	if (RepairParticles)
	{
		RepairParticleComponent = UGameplayStatics::SpawnEmitterAttached(
			RepairParticles,
			RootComponent,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}
}

/*
 * StopRepairEffects
 * - Stops repair loop sound and particle effects
 */
void ARepairableEquipmentActor::StopRepairEffects()
{
	if (RepairAudioComponent)
	{
		RepairAudioComponent->Stop();
		RepairAudioComponent = nullptr;
	}

	if (RepairParticleComponent)
	{
		RepairParticleComponent->DeactivateSystem();
		RepairParticleComponent = nullptr;
	}
}

/*
 * PlayCompletionEffects
 * - Plays sound and VFX upon repair completion
 */
void ARepairableEquipmentActor::PlayCompletionEffects()
{
	if (RepairCompletedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, RepairCompletedSound, GetActorLocation());
	}

	if (CompletionFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			CompletionFlash,
			GetActorLocation(),
			FRotator::ZeroRotator,
			true
		);
	}
}

#pragma endregion

#pragma region Public Methods

/*
 * SetOwningObjective
 * - Assigns the owning repair objective to this equipment
 * 
 * @param Objective The repair objective that owns this equipment
 */
void ARepairableEquipmentActor::SetOwningObjective(URepairObjective* Objective)
{
	OwningObjective = Objective;
}

/*
 * EnableRepair
 * - Enables the repair functionality for this equipment
 */
void ARepairableEquipmentActor::EnableRepair()
{
	bRepairEnabled = true;
}

/*
 * DisableRepair
 * - Disables the repair functionality for this equipment
 * - Cancels any ongoing repair process
 */
void ARepairableEquipmentActor::DisableRepair()
{
	bRepairEnabled = false;
	
	if (bIsRepairing)
	{
		CancelRepair();
	}
}

/*
 * GetRepairProgress
 * - Returns the current repair progress as a float between 0.0 and 1.0
 * 
 * @return float The repair progress ratio
 */
float ARepairableEquipmentActor::GetRepairProgress() const
{
	if (RepairDuration <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(CurrentRepairTime / RepairDuration, 0.0f, 1.0f);
}

#pragma endregion