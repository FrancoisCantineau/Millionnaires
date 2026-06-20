#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TraversalInterface.h"
#include "InteractionInterface.h"
#include "Component/TraversalComponent.h"
#include "TraversalActor.generated.h"

class UArrowComponent;

USTRUCT(BlueprintType)
struct FTraversalEntryInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<USceneComponent> EntryPoint = nullptr;

	UPROPERTY()
	TObjectPtr<UAnimMontage> EnterTransition = nullptr;
};

UENUM(BlueprintType)
enum class ETraversalDirectionSource : uint8
{
	FixedLocal,    
	FixedWorld,      
	PlayerForward,     
	PlayerUp,          
	PlayerRight,
	PlayerLeft,
	PlayerDown,
	Custom           
};


UCLASS(Abstract)
class TRAVERSALSYSTEM_API ATraversalActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	ATraversalActor();

protected:

// SETUP //
	UPROPERTY(EditAnywhere, Category = "Traversal")
	float ObstacleTraceDistance = 50.f;

	UPROPERTY(EditAnywhere, Category = "Traversal")
	float ExitTraceDistance = 100.f;

	UPROPERTY(EditAnywhere, Category = "Traversal")
	float ExitPointDistance = 120.f;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Traversal")
	TObjectPtr<UContextDataAsset> TraversalContextDefinition;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Traversal")
	ETraversalType TraversalType = ETraversalType::None;

	UPROPERTY(EditAnywhere, Category = "Traversal")
	ETraversalDirectionSource DirectionSource = ETraversalDirectionSource::FixedLocal;

	UPROPERTY(EditAnywhere, Category = "Traversal", meta = (EditCondition = "DirectionSource == ETraversalDirectionSource::FixedLocal || DirectionSource == ETraversalDirectionSource::FixedWorld"))
	FVector TraversalDirection = FVector::UpVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	UArrowComponent* StartPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	UArrowComponent* EndPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	UArrowComponent* ExitPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Traversal|Anim")
	FTraversalAnimSet AnimSet;

	virtual void MoveForward(UTraversalComponent*);
	virtual void MoveBackward(UTraversalComponent*);
	virtual void MoveLeft(UTraversalComponent*);
	virtual void MoveRight(UTraversalComponent*);

public:
	
	ETraversalType GetTraversalType()   const { return TraversalType; }
	FVector        GetTraversalAxis()   const { return TraversalDirection; }
	UArrowComponent* GetStartPoint()    const { return StartPoint; }
	UArrowComponent* GetEndPoint()      const { return EndPoint; }
	UArrowComponent* GetExitPoint()     const { return ExitPoint; }
	TObjectPtr<UContextDataAsset> GetContextData () const { return TraversalContextDefinition;}
	virtual FTraversalEntryInfo GetEntryInfo(ACharacter* Character, UTraversalComponent* Component) const;

	USceneComponent* GetEntryPointForCharacter(ACharacter* Character);
	
	virtual UAnimMontage* GetMontageForContext(float Input,ETraversalHand Hand,bool bExiting,bool bEnter, bool bIsEntry, bool bIsExit) const;

	virtual void HandleTraversalNotify(ETraversalNotifyType EventType,class UTraversalComponent* Component);

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionDisplayName_Implementation() const override;

	virtual void HandleTraversalInput(UTraversalComponent* Component,const FVector2D& Input);
	
	virtual FVector GetTraversalDirection(const UTraversalComponent* Component);
};

