#pragma once

#include "CoreMinimal.h"
#include "TraversalActor.h"
#include "LadderActor.generated.h"

class UInstancedStaticMeshComponent;
class UBoxComponent;

UCLASS()
class TRAVERSALSYSTEM_API ALadderActor : public ATraversalActor
{
    GENERATED_BODY()

public:
    ALadderActor();

protected:
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* InteractionVolume;

    UPROPERTY(VisibleAnywhere)
    UInstancedStaticMeshComponent* StepInstances;

    UPROPERTY(EditAnywhere, Category = "Ladder")
    UStaticMesh* StepMesh;

    UPROPERTY(EditAnywhere, Category = "Ladder")
    int32 StepsCount = 8;

    UPROPERTY(EditAnywhere, Category = "Ladder")
    float StepSpacing = 30.f;

    virtual void OnConstruction(const FTransform& Transform) override;

    virtual void MoveForward(UTraversalComponent*) override;
    virtual void MoveBackward(UTraversalComponent*) override;
    
public:
    
    virtual void HandleTraversalNotify(
        ETraversalNotifyType EventType,
        UTraversalComponent* Component
    ) override;

    virtual FText GetInteractionDisplayName_Implementation() const override;

    virtual void HandleTraversalInput(UTraversalComponent* Component, const FVector2D& Input) override;

    virtual FTraversalEntryInfo GetEntryInfo(ACharacter* Character, UTraversalComponent* Component) const override;
};