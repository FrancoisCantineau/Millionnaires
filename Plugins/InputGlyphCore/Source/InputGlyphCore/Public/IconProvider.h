#pragma once


#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputAction.h"
#include "IconProvider.generated.h"

UINTERFACE()
class UIconProvider : public UInterface
{
	GENERATED_BODY()
};

class IIconProvider
{
	GENERATED_BODY()

public:
	virtual UTexture2D* GetIconForKey(FKey Key) const = 0;
	virtual UTexture2D* GetIconForAction(const UInputAction* Action) const { return nullptr; }
};