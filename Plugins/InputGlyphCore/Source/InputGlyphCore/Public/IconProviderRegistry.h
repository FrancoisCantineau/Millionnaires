#pragma once

#include "CoreMinimal.h"

class IIconProvider;

class INPUTGLYPHCORE_API FIconProviderRegistry
{
public:
	static void Register(IIconProvider* Provider);
	static IIconProvider* Get();

private:
	static IIconProvider* Instance;
};