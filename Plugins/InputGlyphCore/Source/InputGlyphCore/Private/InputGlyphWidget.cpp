// Fill out your copyright notice in the Description page of Project Settings.


#include "InputGlyphWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"

#include "IconProvider.h"
#include "IconProviderRegistry.h"

void UInputGlyphWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (GlyphFillImage && FillMaterialBase)
	{
		FillMaterial = UMaterialInstanceDynamic::Create(FillMaterialBase, this);
		GlyphFillImage->SetBrushFromMaterial(FillMaterial);
	}
}

void UInputGlyphWidget::SetInputAction(UInputAction* Action)
{
	CurrentAction = Action;

	if (!Action)
		return;

	IIconProvider* Provider = FIconProviderRegistry::Get();
	if (!Provider)
		return;

	UTexture2D* Texture = Provider->GetIconForAction(Action);
	if (!Texture)
		return;
	
	if (GlyphImage)
		GlyphImage->SetBrushFromTexture(Texture);
	
	if (FillMaterial)
		FillMaterial->SetTextureParameterValue(TEXT("GlyphTexture"), Texture);
}

void UInputGlyphWidget::SetProgress(float Value)
{
	if (FillMaterial)
	{
		FillMaterial->SetScalarParameterValue(TEXT("FillAmount"), Value);
	}
}

void UInputGlyphWidget::PlaySuccess()
{
	// plus tard :
	// animation verte
}

void UInputGlyphWidget::PlayFailure()
{
	// plus tard :
	// shake rouge
}