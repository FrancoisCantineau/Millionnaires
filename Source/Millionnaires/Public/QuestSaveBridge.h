#pragma once
 
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/ISaveable.h"
#include "QuestSaveBridge.generated.h"
 
/**
 * PROJECT CODE - not part of QuestSystem or SaveFramework, and deliberately not moved into
 * either. This is the only place allowed to depend on both plugins at once: it implements
 * SaveFramework's ISaveable and forwards to QuestSystem's own CaptureSaveData()/RestoreSaveData(),
 * so neither plugin ever needs to know the other exists.
 *
 * Solo-game assumption: finds "the" player's QuestComponent via the single local PlayerState.
 * If this project ever goes multiplayer, this bridge would need one saveable entry per player
 * instead of a single global one - not needed today, not built.
 */
UCLASS()
class MILLIONNAIRES_API UQuestSaveBridge : public UGameInstanceSubsystem, public ISaveable
{
	GENERATED_BODY()
 
public:
 
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
 
	//~ ISaveable
	virtual FInstancedStruct CaptureState_Implementation() const override;
	virtual void RestoreState_Implementation(const FInstancedStruct& InState) override;
	//~ End ISaveable
 
	//~ UObject interface
	virtual UWorld* GetWorld() const override;
	//~ End UObject interface
 
private:
 
	/** Solo-game assumption - see class comment. */
	class UQuestComponent* GetLocalQuestComponent() const;
};