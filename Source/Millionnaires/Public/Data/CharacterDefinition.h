/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "CharacterDefinition" - Header
 * Notes: Data asset describing a playable or AI character archetype (name, stats, meshes, anim blueprints, portrait).
 */

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterDefinition.generated.h"

class UTexture2D;
class USkeletalMesh;
class UAnimInstance;

UCLASS(BlueprintType)
class MILLIONNAIRES_API UCharacterDefinition : public UDataAsset
{
    GENERATED_BODY()

public:

    UCharacterDefinition();

protected:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (ToolTip = "In-game display name for this character."))
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (ToolTip = "Short description or role of the character."))
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (ToolTip = "Portrait texture displayed in character selection UI."))
    TObjectPtr<UTexture2D> Portrait;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0.0", ToolTip = "Maximum health points for this character."))
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0.0", ToolTip = "Maximum hunger value for this character (typically only used for player characters)."))
    float MaxHunger = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual|World", meta = (ToolTip = "World-space mesh used to represent this character in the level."))
    TObjectPtr<USkeletalMesh> WorldMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual|World", meta = (ToolTip = "Animation blueprint class used for the world mesh."))
    TSubclassOf<UAnimInstance> WorldAnimClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual|FirstPerson", meta = (ToolTip = "First person arms mesh used by the owning player."))
    TObjectPtr<USkeletalMesh> FirstPersonMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual|FirstPerson", meta = (ToolTip = "Animation blueprint class used for the first person arms mesh."))
    TSubclassOf<UAnimInstance> FirstPersonAnimClass;

public:

    UFUNCTION(BlueprintPure, Category = "Character")
    FText GetDisplayName() const { return DisplayName; }

    UFUNCTION(BlueprintPure, Category = "Character")
    FText GetDescription() const { return Description; }

    UFUNCTION(BlueprintPure, Category = "Character")
    UTexture2D* GetPortrait() const { return Portrait; }

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetMaxHunger() const { return MaxHunger; }

    UFUNCTION(BlueprintPure, Category = "Visual|World")
    USkeletalMesh* GetWorldMesh() const { return WorldMesh; }

    UFUNCTION(BlueprintPure, Category = "Visual|World")
    TSubclassOf<UAnimInstance> GetWorldAnimClass() const { return WorldAnimClass; }

    UFUNCTION(BlueprintPure, Category = "Visual|FirstPerson")
    USkeletalMesh* GetFirstPersonMesh() const { return FirstPersonMesh; }

    UFUNCTION(BlueprintPure, Category = "Visual|FirstPerson")
    TSubclassOf<UAnimInstance> GetFirstPersonAnimClass() const { return FirstPersonAnimClass; }
};
