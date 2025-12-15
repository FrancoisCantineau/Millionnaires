/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchHologramActor" - Header
 * Notes: Example interactable used as the map hologram inside the Dispatch room.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dispatch/DispatchInteractableInterface.h"
#include "DispatchHologramActor.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;
class ADispatchPlayerController;

/** Delegate fired when this hologram is clicked. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDispatchHologramClicked);

/**
 * Simple interactable hologram that opens the Dispatch map when clicked.
 * It also exposes hover feedback through material overrides.
 */
UCLASS()
class MILLIONNAIRES_API ADispatchHologramActor : public AActor, public IDispatchInteractableInterface
{
    GENERATED_BODY()

public:
    /** Default constructor. */
    ADispatchHologramActor();

#pragma region COMPONENTS

protected:
    /** Static mesh representing the hologram pedestal / projector. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dispatch|Hologram", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* MeshComponent;

#pragma endregion COMPONENTS

#pragma region CONFIG

protected:
    /** Optional material used when the hologram is hovered. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Hologram|Visual")
    UMaterialInterface* HoverMaterial;

    /** Optional material restored when the hologram is no longer hovered. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Hologram|Visual")
    UMaterialInterface* DefaultMaterial;

    /** If true, clicking the hologram will automatically toggle the Dispatch map on the player controller. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Hologram")
    bool bAutoToggleMapOnClick = true;

#pragma endregion CONFIG

#pragma region DELEGATES

public:
    /** Event fired whenever the hologram is clicked. */
    UPROPERTY(BlueprintAssignable, Category = "Dispatch|Hologram")
    FDispatchHologramClicked OnHologramClicked;

#pragma endregion DELEGATES

#pragma region IDispatchInteractableInterface

public:
    /** Called when the hologram starts being hovered by the Dispatch cursor. */
    virtual void OnDispatchHoverStarted_Implementation(APlayerController* Controller) override;

    /** Called when the hologram stops being hovered by the Dispatch cursor. */
    virtual void OnDispatchHoverEnded_Implementation(APlayerController* Controller) override;

    /** Called when the hologram is clicked by the Dispatch cursor. */
    virtual void OnDispatchClicked_Implementation(APlayerController* Controller) override;

#pragma endregion IDispatchInteractableInterface
};
