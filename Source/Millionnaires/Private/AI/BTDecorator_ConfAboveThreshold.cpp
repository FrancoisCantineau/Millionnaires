// BTDecorator_ConfAboveThreshold.cpp
#include "AI/BTDecorator_ConfAboveThreshold.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTDecorator_ConfAboveThreshold::UBTDecorator_ConfAboveThreshold()
{
	NodeName = TEXT("Confidence Above Threshold");
	ConfidenceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_ConfAboveThreshold, ConfidenceKey));
}

bool UBTDecorator_ConfAboveThreshold::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return false;
	}

	const float CurrentConfidence = BlackboardComp->GetValueAsFloat(ConfidenceKey.SelectedKeyName);
	return CurrentConfidence > Threshold;
}

#if WITH_EDITOR
FString UBTDecorator_ConfAboveThreshold::GetStaticDescription() const
{
	return FString::Printf(TEXT("Confidence > %.2f"), Threshold);
}
#endif