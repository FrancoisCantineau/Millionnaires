/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchPlayerController" - Header
 * Notes: Dispatch control room PlayerController (input orchestration only).
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DispatchPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

class UDispatchCameraManagerComponent;
class UDispatchCursorComponent;
class UDispatchUIManagerComponent;
class UDispatchMissionManagerComponent;

/**
 * Dispatch PlayerController.
 * - Binds input
 * - Routes camera requests to CameraManager
 * - Routes click to CursorComponent
 * - Owns UI through UIManager (no direct widget calls)
 * - Owns mission system through MissionManager (Day 1 start, offers, dispatch missions)
 */
UCLASS()
class MILLIONNAIRES_API ADispatchPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
#pragma region LIFECYCLE

    /** Constructor. */
    ADispatchPlayerController();

    /** BeginPlay. */
    virtual void BeginPlay() override;

    /** SetupInputComponent. */
    virtual void SetupInputComponent() override;

#pragma endregion LIFECYCLE

#pragma region GETTERS

    /** Camera manager accessor. */
    UFUNCTION(BlueprintPure, Category="Dispatch|Camera")
    UDispatchCameraManagerComponent* GetCameraManager() const { return cameraManagerComponent; }

    /** Cursor component accessor. */
    UFUNCTION(BlueprintPure, Category="Dispatch|Cursor")
    UDispatchCursorComponent* GetCursorComponent() const { return cursorComponent; }

    /** UI manager accessor. */
    UFUNCTION(BlueprintPure, Category="Dispatch|UI")
    UDispatchUIManagerComponent* GetUIManager() const { return uiManagerComponent; }

    /** Mission manager accessor. */
    UFUNCTION(BlueprintPure, Category="Dispatch|Missions")
    UDispatchMissionManagerComponent* GetMissionManager() const { return missionManagerComponent; }

#pragma endregion GETTERS

protected:
#pragma region INPUT_CALLBACKS

    /** Next camera. */
    void HandleNextCamera();

    /** Previous camera. */
    void HandlePrevCamera();

    /** Zoom pressed. */
    void HandleZoomPressed();

    /** Zoom released. */
    void HandleZoomReleased();

    /** Click pressed. */
    void HandleClick();

    /** Toggle map input. */
    void HandleToggleMap();

    /** Close map input. */
    void HandleCloseMap();

#pragma endregion INPUT_CALLBACKS

#pragma region COMPONENTS

    /** Camera manager component. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Components", meta=(AllowPrivateAccess="true"))
    UDispatchCameraManagerComponent* cameraManagerComponent = nullptr;

    /** Cursor component. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Components", meta=(AllowPrivateAccess="true"))
    UDispatchCursorComponent* cursorComponent = nullptr;

    /** UI manager component. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Components", meta=(AllowPrivateAccess="true"))
    UDispatchUIManagerComponent* uiManagerComponent = nullptr;

    /** Mission manager component. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Components", meta=(AllowPrivateAccess="true"))
    UDispatchMissionManagerComponent* missionManagerComponent = nullptr;

#pragma endregion COMPONENTS

#pragma region INPUT

    /** Dispatch input mapping context. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Input", meta=(AllowPrivateAccess="true"))
    UInputMappingContext* dispatchIMC = nullptr;

    /** Next camera action. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Input", meta=(AllowPrivateAccess="true"))
    UInputAction* nextCameraAction = nullptr;

    /** Prev camera action. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Input", meta=(AllowPrivateAccess="true"))
    UInputAction* prevCameraAction = nullptr;

    /** Zoom action (hold). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Input", meta=(AllowPrivateAccess="true"))
    UInputAction* zoomAction = nullptr;

    /** Click/interact action. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Input", meta=(AllowPrivateAccess="true"))
    UInputAction* clickAction = nullptr;

    /** Toggle map action (optional). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Input", meta=(AllowPrivateAccess="true"))
    UInputAction* toggleMapAction = nullptr;

    /** Close map action (optional, e.g. Escape). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Input", meta=(AllowPrivateAccess="true"))
    UInputAction* closeMapAction = nullptr;

#pragma endregion INPUT
};
