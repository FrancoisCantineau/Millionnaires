/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionWorldNotificationWidget" - Source
 * Notes: Base widget for world-space mission notifications (radial timer + icon + title).
 */
#include "Dispatch/UI/Widgets/Missions/World/DispatchMissionWorldNotificationWidget.h"

#include "Dispatch/Missions/DispatchMissionDefinition.h"

void UDispatchMissionWorldNotificationWidget::SetFromOffer(const FDispatchMissionOffer& Offer)
{
    timeRemainingSec = Offer.timeRemainingSec;
    timeLimitSec = Offer.timeLimitSec;

    progress01 = (timeLimitSec > 0.f) ? FMath::Clamp(timeRemainingSec / timeLimitSec, 0.f, 1.f) : 0.f;

    if (Offer.definition)
    {
        titleText = Offer.definition->title;
        iconTexture = Offer.definition->icon;
    }
    else
    {
        titleText = FText::FromString(TEXT("Mission"));
        iconTexture = nullptr;
    }

    const float Display01 = bInvertRadialFill ? (1.f - progress01) : progress01;

    // Notify Blueprint so the WBP can update any controls (RadialSlider, images, texts...) without C++ dependencies.
    BP_OnProgress01Changed(Display01);
    BP_OnIconChanged(iconTexture);
    BP_OnTitleChanged(titleText);
    BP_OnTimeChanged(timeRemainingSec, timeLimitSec);
    BP_OnOfferUpdated();
}
