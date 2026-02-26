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
#include "GameplayAbilitySystem/Data/AbilityInfosStruct.h"
#include "Dispatch/Missions/DispatchMissionTypes.h"
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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Profile", meta = (ToolTip = "Stable identifier used when selecting this character for missions."))
    FName CharacterId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Profile", meta = (ToolTip = "Skill ratings (0..10) used for mission success computation."))
    TMap<EDispatchMissionSkill, int32> DispatchSkillRatings10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Profile", meta = (ToolTip = "Affinity multiplier per mission location. 1.0 means neutral, >1 better, <1 worse."))
    TMap<EDispatchMissionLocation, float> LocationAffinityMultipliers;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Profile", meta = (ClampMin = "0.0", ClampMax = "1.0", ToolTip = "Flat bonus added to mission success chance for this character (0..1)."))
    float FlatSuccessBonus01 = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dispatch|Profile", meta = (ClampMin = "0.0", ClampMax = "1.0", ToolTip = "Equipment bonus placeholder (0..1). Later this should come from equipped items/inventory."))
    float EquipmentBonus01 = 0.0f;

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

    UFUNCTION(BlueprintPure, Category = "Dispatch|Profile")
    FName GetCharacterId() const { return CharacterId; }

    UFUNCTION(BlueprintPure, Category = "Dispatch|Profile")
    int32 GetSkillRating10(EDispatchMissionSkill Skill) const
    {
        const int32* Found = DispatchSkillRatings10.Find(Skill);
        return Found ? *Found : 0;
    }

    UFUNCTION(BlueprintPure, Category = "Dispatch|Profile")
    float GetLocationAffinityMultiplier(EDispatchMissionLocation Location) const
    {
        const float* Found = LocationAffinityMultipliers.Find(Location);
        return Found ? *Found : 1.0f;
    }

    UFUNCTION(BlueprintPure, Category = "Dispatch|Profile")
    float GetFlatSuccessBonus01() const { return FlatSuccessBonus01; }

    UFUNCTION(BlueprintPure, Category = "Dispatch|Profile")
    float GetEquipmentBonus01() const { return EquipmentBonus01; }

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
    
        
    /** GAS */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
    TArray<FAbilityInfosStruct> Abilities;

    /** AI COMBAT */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float MinStraffingDistance = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float MaxStraffingDistance = 0;
};
