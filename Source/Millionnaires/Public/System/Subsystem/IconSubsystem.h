#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "IconProvider.h"
#include "IconProviderRegistry.h"
#include "Data/KeyIconSet.h"
#include "IconSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIconSystemReady);

UCLASS()

class UIconSubsystem : public UGameInstanceSubsystem, public IIconProvider
{
	GENERATED_BODY()

private:
	
	UPROPERTY()
	TObjectPtr<UKeyIconSet> IconSet;

public:

	UPROPERTY(BlueprintAssignable)
	FOnIconSystemReady OnReady;
	
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		Super::Initialize(Collection);
		FIconProviderRegistry::Register(this);

		OnReady.Broadcast();
	}

	virtual void Deinitialize() override
	{
		FIconProviderRegistry::Register(nullptr);
		Super::Deinitialize();
	}

	UFUNCTION(BlueprintCallable)
	void SetIconSet(UKeyIconSet* InSet)
	{
		IconSet = InSet;
	}

	virtual UTexture2D* GetIconForKey(FKey Key) const override
	{
		return IconSet ? IconSet->GetIconForKey(Key) : nullptr;
	}

	virtual UTexture2D* GetIconForAction(const UInputAction* Action) const override;
};