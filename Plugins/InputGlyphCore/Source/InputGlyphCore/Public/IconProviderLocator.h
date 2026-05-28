#pragma once

#include "CoreMinimal.h"

class IIconProvider;

class FIconProviderLocator
{
public:
    static IIconProvider* Get(UObject* WorldContext);
};