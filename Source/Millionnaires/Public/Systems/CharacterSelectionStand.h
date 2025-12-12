/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "CharacterSelectionStand" - Header
 * Notes: World actor used to display a character preview and handle in-world selection via mouse hover/click.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterSelectionStand.generated.h"

class USceneComponent;
class USkeletalMeshComponent;
class USpotLightComponent;
class UCharacterDefinition;

UCLASS()
class MILLIONNAIRES_API ACharacterSelectionStand : public AActor
{
    GENERATED_BODY()

public:

    ACharacterSelectionStand();

protected:

    /** Root scene component for the selection stand. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", ToolTip = "Root scene component for the selection stand."))
    TObjectPtr<USceneComponent> Root;

    /** Skeletal mesh component previewing the character on this stand. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", ToolTip = "Skeletal mesh component previewing the character on this stand."))
    TObjectPtr<USkeletalMeshComponent> PreviewMesh;

    /** Spot light used to indicate which character is hovered or selected. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", ToolTip = "Spot light used to indicate which character is hovered or selected."))
    TObjectPtr<USpotLightComponent> SelectionLight;

    /** Character definition represented by this stand. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (ToolTip = "Character definition represented by this stand."))
    TObjectPtr<UCharacterDefinition> CharacterDefinition;

    /** Light color used when the stand is hovered but not yet selected. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", meta = (ToolTip = "Light color used when the stand is hovered but not yet selected."))
    FLinearColor HoverLightColor = FLinearColor::White;

    /** Light color used when the stand is selected. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", meta = (ToolTip = "Light color used when the stand is selected."))
    FLinearColor SelectedLightColor = FLinearColor::Green;

    /** Light intensity when the stand is hovered. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", meta = (ClampMin = "0.0", ToolTip = "Light intensity when the stand is hovered."))
    float HoverLightIntensity = 5000.0f;

    /** Light intensity when the stand is selected. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", meta = (ClampMin = "0.0", ToolTip = "Light intensity when the stand is selected."))
    float SelectedLightIntensity = 8000.0f;

    /** If true, the stand will log hover and selection events. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (ToolTip = "If true, the stand will log hover and selection events."))
    bool bDebug = false;

    bool bIsSelected = false;
    bool bIsHovered = false;

    virtual void BeginPlay() override;
    virtual void NotifyActorBeginCursorOver() override;
    virtual void NotifyActorEndCursorOver() override;
    virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

    /** Applies the character definition mesh and anim class to the preview mesh. */
    void ApplyVisualsFromDefinition();

    /** Updates the selection light based on hover/selection state. */
    void UpdateLight();

    /** Selects this stand and notifies the character selection subsystem. */
    void SelectThisStand();
};
