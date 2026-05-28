#include "System/Subsystem/IconSubsystem.h"

#include "EnhancedInputSubsystems.h"

UTexture2D* UIconSubsystem::GetIconForAction(const UInputAction* Action) const
{
	if (!Action || !IconSet) return nullptr;

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return nullptr;

	ULocalPlayer* LP = PC->GetLocalPlayer();
	if (!LP) return nullptr;

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP);

	if (!Subsystem) return nullptr;

	TArray<FKey> Keys = Subsystem->QueryKeysMappedToAction(Action);

	if (Keys.Num() > 0)
	{
		return IconSet->GetIconForKey(Keys[0]);
	}

	return nullptr;
}
