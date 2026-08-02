#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Event/GameplayEventRegistry.h"
#include "Event/GameplayEventSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameplayEventRegistryComponent.generated.h"

/**
 * Place on a level actor, to save a UGameplayEventRegistry, to the global UGameplayEventSubsystem.
 */
UCLASS(ClassGroup = (GameplayOrchestrator), meta = (BlueprintSpawnableComponent))
class GAMEPLAYORCHESTRATOR_API UGameplayEventRegistryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGameplayEventRegistryComponent() { PrimaryComponentTick.bCanEverTick = false; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Orchestrator")
	TObjectPtr<UGameplayEventRegistry> Registry;

protected:
	virtual void BeginPlay() override
	{
		Super::BeginPlay();
		if (Registry)
		{
			if (AActor* Owner = GetOwner())
			{
				if (UGameInstance* GameInstance = Owner->GetGameInstance())
				{
					if (UGameplayEventSubsystem* EventSubsystem = GameInstance->GetSubsystem<UGameplayEventSubsystem>())
					{
						EventSubsystem->RegisterEventRegistry(Registry);
					}
				}
			}
		}
	}

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override
	{
		if (Registry)
		{
			if (AActor* Owner = GetOwner())
			{
				if (UGameInstance* GameInstance = Owner->GetGameInstance())
				{
					if (UGameplayEventSubsystem* EventSubsystem = GameInstance->GetSubsystem<UGameplayEventSubsystem>())
					{
						EventSubsystem->UnregisterEventRegistry(Registry);
					}
				}
			}
		}
		Super::EndPlay(EndPlayReason);
	}
};