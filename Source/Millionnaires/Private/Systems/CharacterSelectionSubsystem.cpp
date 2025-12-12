/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "CharacterSelectionSubsystem" - Source
 * Notes: Game instance subsystem storing and broadcasting the currently selected CharacterDefinition.
 */

#include "Systems/CharacterSelectionSubsystem.h"

#include "Data/CharacterDefinition.h"
#include "Millionnaires.h"

UCharacterSelectionSubsystem::UCharacterSelectionSubsystem()
    : Super()
{
}

void UCharacterSelectionSubsystem::SetSelectedCharacter(UCharacterDefinition* NewSelection)
{
    if (SelectedCharacter == NewSelection)
    {
        return;
    }

    SelectedCharacter = NewSelection;

    if (SelectedCharacter)
    {
        UE_LOG(LogMillionnaires, Log, TEXT("[CharSelectSubsystem] Selected character set to '%s'."),
            *SelectedCharacter->GetDisplayName().ToString());
    }
    else
    {
        UE_LOG(LogMillionnaires, Log, TEXT("[CharSelectSubsystem] Selected character cleared."));
    }

    OnSelectedCharacterChanged.Broadcast(SelectedCharacter);
}
