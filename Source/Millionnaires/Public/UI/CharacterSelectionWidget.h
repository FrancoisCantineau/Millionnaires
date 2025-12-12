/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "CharacterSelectionWidget" - Header
 * Notes: Optional UMG widget displaying a list of CharacterDefinition assets and exposing a selection delegate.
 */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectionWidget.generated.h"

class UCharacterDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterPickedSignature, UCharacterDefinition*, PickedCharacter);

UCLASS(Abstract, Blueprintable)
class MILLIONNAIRES_API UCharacterSelectionWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    UCharacterSelectionWidget(const FObjectInitializer& ObjectInitializer);

protected:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Characters", meta = (ToolTip = "List of character definitions displayed by this selection widget."))
    TArray<TObjectPtr<UCharacterDefinition>> AvailableCharacters;

public:

    UPROPERTY(BlueprintAssignable, Category = "Characters|Delegates")
    FOnCharacterPickedSignature OnCharacterPicked;

    UFUNCTION(BlueprintPure, Category = "Characters")
    void GetAvailableCharacters(TArray<UCharacterDefinition*>& OutCharacters) const;

    UFUNCTION(BlueprintCallable, Category = "Characters")
    void SetAvailableCharacters(const TArray<UCharacterDefinition*>& NewCharacters);

    UFUNCTION(BlueprintCallable, Category = "Characters")
    void SelectCharacterByIndex(int32 Index);

protected:

    UFUNCTION(BlueprintImplementableEvent, Category = "Characters")
    void HandleCharacterSelected(UCharacterDefinition* SelectedDefinition);
};
