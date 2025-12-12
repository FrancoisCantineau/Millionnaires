/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "CharacterSelectionWidget" - Source
 * Notes: Optional UMG widget displaying a list of CharacterDefinition assets and exposing a selection delegate.
 */

#include "UI/CharacterSelectionWidget.h"

#include "Data/CharacterDefinition.h"
#include "Systems/CharacterSelectionSubsystem.h"
#include "Engine/GameInstance.h"

UCharacterSelectionWidget::UCharacterSelectionWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UCharacterSelectionWidget::GetAvailableCharacters(TArray<UCharacterDefinition*>& OutCharacters) const
{
    OutCharacters.Reset();

    for (UCharacterDefinition* Definition : AvailableCharacters)
    {
        OutCharacters.Add(Definition);
    }
}

void UCharacterSelectionWidget::SetAvailableCharacters(const TArray<UCharacterDefinition*>& NewCharacters)
{
    AvailableCharacters.Reset();

    for (UCharacterDefinition* Definition : NewCharacters)
    {
        AvailableCharacters.Add(Definition);
    }
}

void UCharacterSelectionWidget::SelectCharacterByIndex(int32 Index)
{
    if (!AvailableCharacters.IsValidIndex(Index))
    {
        return;
    }

    UCharacterDefinition* SelectedDefinition = AvailableCharacters[Index];

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UCharacterSelectionSubsystem* SelectionSubsystem = GameInstance->GetSubsystem<UCharacterSelectionSubsystem>())
        {
            SelectionSubsystem->SetSelectedCharacter(SelectedDefinition);
        }
    }

    OnCharacterPicked.Broadcast(SelectedDefinition);
    HandleCharacterSelected(SelectedDefinition);
}
