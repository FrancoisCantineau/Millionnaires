// BTService_DecayConfidence.cpp
#include "AI/BTService_DecayConfidence.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTService_DecayConfidence::UBTService_DecayConfidence()
{
	NodeName = TEXT("Decay Confidence");
	Interval = 0.2f;
	RandomDeviation = 0.f;
	ConfidenceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_DecayConfidence, ConfidenceKey));
}

void UBTService_DecayConfidence::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	const float CurrentConfidence = BlackboardComp->GetValueAsFloat(ConfidenceKey.SelectedKeyName);
	const float NewConfidence = FMath::Clamp(CurrentConfidence - (DeltaSeconds / DecayDurationSeconds), 0.f, 1.f);
	BlackboardComp->SetValueAsFloat(ConfidenceKey.SelectedKeyName, NewConfidence);
}