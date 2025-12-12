/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "CharacterSelectionStand" - Source
 * Notes: World actor used to display a character preview and handle in-world selection via mouse hover/click.
 */

#include "Systems/CharacterSelectionStand.h"

#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Data/CharacterDefinition.h"
#include "Systems/CharacterSelectionSubsystem.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Millionnaires.h"

ACharacterSelectionStand::ACharacterSelectionStand()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    PreviewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
    PreviewMesh->SetupAttachment(RootComponent);

    // Mesh is only used for cursor hit tests
    PreviewMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PreviewMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    PreviewMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    PreviewMesh->SetGenerateOverlapEvents(false);

    SelectionLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SelectionLight"));
    SelectionLight->SetupAttachment(RootComponent);
    SelectionLight->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
    SelectionLight->SetIntensity(0.0f);
    SelectionLight->SetLightColor(HoverLightColor);
    SelectionLight->SetVisibility(true);
}

void ACharacterSelectionStand::BeginPlay()
{
    Super::BeginPlay();

    ApplyVisualsFromDefinition();
    UpdateLight();
}

void ACharacterSelectionStand::NotifyActorBeginCursorOver()
{
    Super::NotifyActorBeginCursorOver();

    bIsHovered = true;
    UpdateLight();

    if (bDebug)
    {
        UE_LOG(LogMillionnaires, Log, TEXT("[SelectionStand] Hover begin on '%s'."), *GetName());
    }
}

void ACharacterSelectionStand::NotifyActorEndCursorOver()
{
    Super::NotifyActorEndCursorOver();

    bIsHovered = false;
    UpdateLight();

    if (bDebug)
    {
        UE_LOG(LogMillionnaires, Log, TEXT("[SelectionStand] Hover end on '%s'."), *GetName());
    }
}

void ACharacterSelectionStand::NotifyActorOnClicked(FKey ButtonPressed)
{
    Super::NotifyActorOnClicked(ButtonPressed);

    if (bDebug)
    {
        UE_LOG(LogMillionnaires, Log, TEXT("[SelectionStand] Clicked '%s'."), *GetName());
    }

    SelectThisStand();
}

void ACharacterSelectionStand::ApplyVisualsFromDefinition()
{
    if (!CharacterDefinition || !PreviewMesh)
    {
        return;
    }

    if (USkeletalMesh* WorldMesh = CharacterDefinition->GetWorldMesh())
    {
        PreviewMesh->SetSkeletalMesh(WorldMesh);
    }

    if (TSubclassOf<UAnimInstance> AnimClass = CharacterDefinition->GetWorldAnimClass())
    {
        PreviewMesh->SetAnimInstanceClass(AnimClass);
    }
}

void ACharacterSelectionStand::UpdateLight()
{
    if (!SelectionLight)
    {
        return;
    }

    if (bIsSelected)
    {
        SelectionLight->SetLightColor(SelectedLightColor);
        SelectionLight->SetIntensity(SelectedLightIntensity);
    }
    else if (bIsHovered)
    {
        SelectionLight->SetLightColor(HoverLightColor);
        SelectionLight->SetIntensity(HoverLightIntensity);
    }
    else
    {
        SelectionLight->SetIntensity(0.0f);
    }
}

void ACharacterSelectionStand::SelectThisStand()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // First clear selection on all stands
    for (TActorIterator<ACharacterSelectionStand> It(World); It; ++It)
    {
        ACharacterSelectionStand* OtherStand = *It;
        if (!OtherStand)
        {
            continue;
        }

        OtherStand->bIsSelected = false;
        OtherStand->UpdateLight();
    }

    // Mark this stand as selected
    bIsSelected = true;
    UpdateLight();

    if (bDebug)
    {
        UE_LOG(LogMillionnaires, Log, TEXT("[SelectionStand] '%s' is now selected."), *GetName());
    }

    // Notify the character selection subsystem
    if (CharacterDefinition)
    {
        if (UGameInstance* GameInstance = GetGameInstance())
        {
            if (UCharacterSelectionSubsystem* SelectionSubsystem = GameInstance->GetSubsystem<UCharacterSelectionSubsystem>())
            {
                SelectionSubsystem->SetSelectedCharacter(CharacterDefinition);
            }
        }
    }
}
