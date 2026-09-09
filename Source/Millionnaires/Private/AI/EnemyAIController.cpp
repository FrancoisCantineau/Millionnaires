// EnemyAIController.cpp
#include "AI/EnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemyAIController::AEnemyAIController()
{
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 1700.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 2000.f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->ConfigureSense(*HearingConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

	SetPerceptionComponent(*AIPerceptionComp);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB || !Actor)
	{
		return;
	}

	// V0 only cares about the player — ignore anything else perception might report.
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (Actor != PlayerPawn)
	{
		return;
	}

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			// Currently seeing the player — perfect, immediate information.
			BB->SetValueAsObject(TargetActorKeyName, Actor);
			BB->SetValueAsVector(LastKnownLocationKeyName, Stimulus.StimulusLocation);
			BB->SetValueAsBool(TargetVisibleKeyName, true);
			BB->SetValueAsFloat(ConfidenceKeyName, 1.0f);
		}
		else
		{
			// Just lost sight — was seeing perfectly a moment ago, so confidence starts high
			// and decays over time via UBTService_DecayConfidence while investigating.
			BB->SetValueAsVector(LastKnownLocationKeyName, Stimulus.StimulusLocation);
			BB->SetValueAsBool(TargetVisibleKeyName, false);
			BB->SetValueAsFloat(ConfidenceKeyName, ConfidenceOnSightLoss);
		}
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			// A noise only means "something happened over there" — much less reliable than
			// sight, and it never claims the player is definitely visible.
			BB->SetValueAsVector(LastKnownLocationKeyName, Stimulus.StimulusLocation);
			BB->SetValueAsBool(TargetVisibleKeyName, false);
			BB->SetValueAsFloat(ConfidenceKeyName, ConfidenceOnNoise);
		}
	}
}