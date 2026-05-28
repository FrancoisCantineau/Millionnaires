// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Blueprint/UserWidget.h"
#include "InputGlyphWidget.generated.h"


class UImage;
class UTexture2D;
/**
 * 
 */
UCLASS()
class INPUTGLYPHCORE_API UInputGlyphWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SetInputAction(UInputAction* Action);

	UFUNCTION(BlueprintCallable)
	void SetProgress(float Value);

	UFUNCTION(BlueprintCallable)
	void PlaySuccess();

	UFUNCTION(BlueprintCallable)
	void PlayFailure();

protected:

	virtual void NativeConstruct() override;

protected:

	UPROPERTY(meta = (BindWidget))
	UImage* GlyphFillImage;

	UPROPERTY()
	UMaterialInstanceDynamic* FillMaterial;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* FillMaterialBase;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> GlyphImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Glyph")
	TObjectPtr<UInputAction> CurrentAction;
};
