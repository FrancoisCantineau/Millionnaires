#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TraversalInterface.h"
#include "InteractionInterface.h"
#include "Component/TraversalComponent.h"
#include "TraversalActor.generated.h"

class UArrowComponent;

UCLASS(Abstract)
class TRAVERSALSYSTEM_API ATraversalActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	ATraversalActor();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Traversal")
	ETraversalType TraversalType = ETraversalType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Traversal")
	FVector TraversalAxis = FVector::UpVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	UArrowComponent* StartPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	UArrowComponent* EndPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traversal")
	UArrowComponent* ExitPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Traversal|Anim")
	FTraversalAnimSet AnimSet;

public:
	
	ETraversalType GetTraversalType()   const { return TraversalType; }
	FVector        GetTraversalAxis()   const { return TraversalAxis; }
	UArrowComponent* GetStartPoint()    const { return StartPoint; }
	UArrowComponent* GetEndPoint()      const { return EndPoint; }
	UArrowComponent* GetExitPoint()     const { return ExitPoint; }
	
	virtual UAnimMontage* GetMontageForContext(
		float Input,
		ETraversalHand Hand,
		bool bExiting,
		bool bEnter, bool bIsEntry, bool bIsExit
	) const;

	virtual void HandleTraversalNotify(
		ETraversalNotifyType EventType,
		class UTraversalComponent* Component
	);

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionDisplayName_Implementation() const override;
};