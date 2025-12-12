/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "CharacterSelectionSubsystem" - Header
 * Notes: Game instance subsystem storing and broadcasting the currently selected CharacterDefinition.
 */

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CharacterSelectionSubsystem.generated.h"

class UCharacterDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedCharacterChangedSignature, UCharacterDefinition*, NewSelection);

UCLASS()
class MILLIONNAIRES_API UCharacterSelectionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    UCharacterSelectionSubsystem();

    /** Sets the selected character definition and notifies listeners if it changed. */
    UFUNCTION(BlueprintCallable, Category = "CharacterSelection")
    void SetSelectedCharacter(UCharacterDefinition* NewSelection);

    /** Returns the currently selected character definition. */
    UFUNCTION(BlueprintPure, Category = "CharacterSelection")
    UCharacterDefinition* GetSelectedCharacter() const { return SelectedCharacter; }

    /** Called whenever the selected character changes. */
    UPROPERTY(BlueprintAssignable, Category = "CharacterSelection|Delegates")
    FOnSelectedCharacterChangedSignature OnSelectedCharacterChanged;

protected:

    /** Currently selected character definition for the current or next run. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterSelection", meta = (ToolTip = "Currently selected character definition for the current or next run."))
    TObjectPtr<UCharacterDefinition> SelectedCharacter = nullptr;
};
