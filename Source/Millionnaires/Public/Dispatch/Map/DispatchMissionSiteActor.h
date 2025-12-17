/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionSiteActor" - Header
 * Notes: Represents a building/zone group that can be targeted by missions and highlighted via Overlay Material.
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dispatch/Missions/DispatchMissionTypes.h"
#include "DispatchMissionSiteActor.generated.h"

class UMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

/** Visual state for a mission site (used for highlights). */
UENUM(BlueprintType)
enum class EDispatchMissionSiteVisualState : uint8
{
    None      UMETA(DisplayName="None"),
    Offer     UMETA(DisplayName="Offer Available"),
    Running   UMETA(DisplayName="Mission Running"),
    Failed    UMETA(DisplayName="Mission Failed"),
    Completed UMETA(DisplayName="Mission Completed"),
};

UCLASS()
class MILLIONNAIRES_API ADispatchMissionSiteActor : public AActor
{
    GENERATED_BODY()

public:
#pragma region LIFECYCLE

    ADispatchMissionSiteActor();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

#pragma endregion LIFECYCLE

#pragma region API

    /** Sets current visual state (offer/running/failed/completed). */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Site")
    void SetVisualState(EDispatchMissionSiteVisualState NewState);

    /** Convenience: called when an offer is created/removed for this site. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Site")
    void SetOfferActive(bool bActive);

    /** Convenience: called when a mission changes state for this site. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Site")
    void SetMissionState(EDispatchMissionState NewMissionState);

    /** Returns the mission location value for this site (used by MissionManager selection). */
    UFUNCTION(BlueprintPure, Category="Dispatch|Map|Site")
    EDispatchMissionLocation GetMissionLocation() const { return missionLocation; }

    /** Forces a rebuild of cached meshes (useful if targets are spawned at runtime). */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Debug")
    void RebuildMeshCache();

#pragma endregion API

#pragma region SETTINGS

    /** Actors whose mesh components will be highlighted (buildings, props, etc). */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Dispatch|Map|Site")
    TArray<TObjectPtr<AActor>> targetActors;

    /** If true, also highlight this actor's components even if targetActors is not empty. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Site")
    bool bIncludeSelfMeshes = true;

    /** Mission location dropdown for this site (must match MissionDefinition). */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Dispatch|Map|Site")
    EDispatchMissionLocation missionLocation = EDispatchMissionLocation::Any;

    /** Overlay material applied while highlighted. If null, highlight is disabled. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Highlight")
    TObjectPtr<UMaterialInterface> highlightOverlayMaterial = nullptr;

    /** Scalar parameter name used to drive highlight alpha (0..1). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Highlight")
    FName highlightAlphaParamName = FName("Alpha");

    /** If false, highlight is applied instantly (no tick / no smooth). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Highlight")
    bool bSmoothFade = true;

    /** Fade in speed (units per second). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Highlight", meta=(ClampMin="0.01"))
    float fadeInSpeed = 2.5f;

    /** Fade out speed (units per second). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Highlight", meta=(ClampMin="0.01"))
    float fadeOutSpeed = 3.5f;

    /** If true, prints debug logs. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Debug")
    bool bDebugLogs = true;

#pragma endregion SETTINGS

protected:
#pragma region INTERNAL

    void CacheMeshes();
    void ApplyOverlayToMeshes(UMaterialInterface* Overlay);
    void RestoreOriginalOverlays();
    void EnsureOverlayMID();

    float GetTargetAlphaForState(EDispatchMissionSiteVisualState State) const;
    void ApplyInstant(float Alpha);

#pragma endregion INTERNAL

private:
#pragma region STATE

    UPROPERTY(Transient)
    EDispatchMissionSiteVisualState visualState = EDispatchMissionSiteVisualState::None;

    float currentAlpha = 0.f;
    float targetAlpha = 0.f;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMeshComponent>> cachedMeshes;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMaterialInterface>> originalOverlayByMesh;

    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> overlayMID = nullptr;

#pragma endregion STATE
};
