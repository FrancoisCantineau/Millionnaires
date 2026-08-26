#include "QuestSaveBridge.h"
#include "SaveFramework/Public/Core/GlobalSaveableRegistrySubsystem.h"
#include "QuestComponent.h"
#include "QuestSaveData.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "StructUtils/InstancedStruct.h"

void UQuestSaveBridge::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
 
	if (UGlobalSaveableRegistrySubsystem* Registry = GetGameInstance()->GetSubsystem<UGlobalSaveableRegistrySubsystem>())
	{
		// "Quests" must stay unique project-wide - see UGlobalSaveableRegistrySubsystem's contract.
		Registry->RegisterGlobalSaveable(TEXT("Quests"), this);
	}
}
 
UWorld* UQuestSaveBridge::GetWorld() const
{
	return GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
}
 
UQuestComponent* UQuestSaveBridge::GetLocalQuestComponent() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
 
	APlayerState* PlayerState = UGameplayStatics::GetPlayerState(World, 0);
	if (!PlayerState)
	{
		return nullptr;
	}
 
	return PlayerState->FindComponentByClass<UQuestComponent>();
}
 
FInstancedStruct UQuestSaveBridge::CaptureState_Implementation() const
{
	UQuestComponent* QuestComponent = GetLocalQuestComponent();
	if (!QuestComponent)
	{
		return FInstancedStruct();
	}
 
	FQuestSaveData SaveData = QuestComponent->CaptureSaveData();
	return FInstancedStruct::Make(SaveData);
}
 
void UQuestSaveBridge::RestoreState_Implementation(const FInstancedStruct& InState)
{
	UQuestComponent* QuestComponent = GetLocalQuestComponent();
	if (!QuestComponent)
	{
		return;
	}
 
	if (const FQuestSaveData* SaveData = InState.GetPtr<FQuestSaveData>())
	{
		QuestComponent->RestoreSaveData(*SaveData);
	}
}