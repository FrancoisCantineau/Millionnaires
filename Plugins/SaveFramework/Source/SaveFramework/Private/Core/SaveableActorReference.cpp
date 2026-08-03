#include "Core/SaveableActorReference.h"
#include "Core/SaveGuidComponent.h"
#include "GameFramework/Actor.h"

#if WITH_EDITOR
void FSaveableActorReference::RefreshCachedId()
{
	CachedId.Invalidate();

	if (AActor* Actor = TargetActor.LoadSynchronous())
	{
		if (const USaveGuidComponent* GuidComp = Actor->FindComponentByClass<USaveGuidComponent>())
		{
			CachedId = GuidComp->GetSaveId();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveFramework: %s has no SaveGuidComponent, it cannot be referenced by GUID."), *Actor->GetName());
		}
	}
}
#endif
