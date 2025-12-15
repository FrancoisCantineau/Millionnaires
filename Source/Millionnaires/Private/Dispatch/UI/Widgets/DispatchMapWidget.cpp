/* 
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMapWidget" - Source
 * Notes: Interactive map widget that displays mission offers as markers and supports pan/zoom.
 */
#include "Dispatch/UI/Widgets/DispatchMapWidget.h"

#include "Dispatch/Missions/DispatchMissionManagerComponent.h"
#include "Dispatch/Missions/DispatchMissionDefinition.h"
#include "Dispatch/Map/DispatchMapBoundsVolume.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "EngineUtils.h"
#include "Input/Reply.h"

#pragma region API_SETUP

void UDispatchMapWidget::SetMissionManager(UDispatchMissionManagerComponent* InMissionManager)
{
    if (missionManager.Get() == InMissionManager)
    {
        return;
    }

    if (missionManager.IsValid())
    {
        missionManager->OnOfferAdded.RemoveDynamic(this, &UDispatchMapWidget::HandleOfferAdded);
        missionManager->OnOfferRemoved.RemoveDynamic(this, &UDispatchMapWidget::HandleOfferRemoved);
    }

    missionManager = InMissionManager;

    if (missionManager.IsValid())
    {
        missionManager->OnOfferAdded.AddDynamic(this, &UDispatchMapWidget::HandleOfferAdded);
        missionManager->OnOfferRemoved.AddDynamic(this, &UDispatchMapWidget::HandleOfferRemoved);
    }

    RefreshOffers();
}

void UDispatchMapWidget::SetMapBoundsVolume(ADispatchMapBoundsVolume* InBoundsVolume)
{
    boundsVolume = InBoundsVolume;
    RefreshOffers();
}

void UDispatchMapWidget::RequestClose()
{
    OnCloseRequested.Broadcast();
}

#pragma endregion API_SETUP

#pragma region API_OFFERS

void UDispatchMapWidget::RefreshOffers()
{
    if (!boundsVolume.IsValid())
    {
        AutoFindBoundsVolume();
    }

    RebuildMarkerData();

    if (bAutoSpawnMarkers)
    {
        SpawnMarkerWidgets();
    }

    HandleMarkersUpdated(markers);
}

void UDispatchMapWidget::SelectOffer(const FGuid& OfferId)
{
    if (!OfferId.IsValid())
    {
        ClearSelection();
        return;
    }

    selectedOfferId = OfferId;
    OnOfferSelected.Broadcast(OfferId);

    for (auto& Pair : markerWidgetsByOfferId)
    {
        if (Pair.Value)
        {
            Pair.Value->SetSelected(Pair.Key == selectedOfferId);
        }
    }

    UpdateDetailsWidget();
}

void UDispatchMapWidget::ClearSelection()
{
    selectedOfferId.Invalidate();

    for (auto& Pair : markerWidgetsByOfferId)
    {
        if (Pair.Value)
        {
            Pair.Value->SetSelected(false);
        }
    }

    if (detailsWidget)
    {
        detailsWidget->RequestVisible(false);
    }
}

#pragma endregion API_OFFERS

#pragma region API_TRANSFORM

void UDispatchMapWidget::AddZoom(float Delta)
{
    zoom = FMath::Clamp(zoom + Delta, minZoom, maxZoom);
    ApplyTransform();
}

void UDispatchMapWidget::AddPan(const FVector2D& Delta)
{
    pan += Delta;
    ApplyTransform();
}

void UDispatchMapWidget::ResetTransform()
{
    zoom = 1.f;
    pan = FVector2D::ZeroVector;
    ApplyTransform();
}

#pragma endregion API_TRANSFORM

#pragma region UUSERWIDGET

void UDispatchMapWidget::NativeConstruct()
{
    Super::NativeConstruct();


    AutoFindBoundsVolume();
    ApplyTransform();
}

void UDispatchMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bNeedsMarkerReposition)
    {
        UpdateMarkerWidgetPositions();
    }
}

FReply UDispatchMapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    const float Wheel = InMouseEvent.GetWheelDelta();
    if (FMath::Abs(Wheel) > KINDA_SMALL_NUMBER)
    {
        AddZoom(Wheel * zoomStep);
        return FReply::Handled();
    }

    return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

FReply UDispatchMapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        bIsPanning = true;
        lastMouse = InMouseEvent.GetScreenSpacePosition();
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UDispatchMapWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        bIsPanning = false;
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UDispatchMapWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsPanning)
    {
        const FVector2D Now = InMouseEvent.GetScreenSpacePosition();
        const FVector2D Delta = Now - lastMouse;
        lastMouse = Now;

        const float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
        const FVector2D DeltaScaled = (Scale > 0.f) ? (Delta / Scale) : Delta;

        AddPan(DeltaScaled);
        return FReply::Handled();
    }

    return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

#pragma endregion UUSERWIDGET

#pragma region INTERNAL

void UDispatchMapWidget::AutoFindBoundsVolume()
{
    if (boundsVolume.IsValid())
    {
        return;
    }

    if (!GetWorld())
    {
        return;
    }

    for (TActorIterator<ADispatchMapBoundsVolume> It(GetWorld()); It; ++It)
    {
        boundsVolume = *It;
        break;
    }
}

void UDispatchMapWidget::RebuildMarkerData()
{
    markers.Reset();

    if (!missionManager.IsValid())
    {
        return;
    }

    const TArray<FDispatchMissionOffer> Offers = missionManager->GetOffers();
    markers.Reserve(Offers.Num());

    for (const FDispatchMissionOffer& Offer : Offers)
    {
        if (!Offer.definition)
        {
            continue;
        }

        FDispatchMapOfferMarker M;
        M.offerId = Offer.offerId;
        M.worldLocation = Offer.worldLocation;
        M.normalizedPos = WorldToNormalized(Offer.worldLocation);
        M.title = Offer.definition->title;
        M.missionType = (int32)Offer.definition->missionType;

        M.monsterChance10 = Offer.difficulty.monsterChance10;
        M.lootChance10 = Offer.difficulty.lootChance10;
        M.complicationChance10 = Offer.difficulty.complicationChance10;

        markers.Add(M);
    }

    if (selectedOfferId.IsValid())
    {
        bool bStillExists = false;
        for (const FDispatchMapOfferMarker& M : markers)
        {
            if (M.offerId == selectedOfferId)
            {
                bStillExists = true;
                break;
            }
        }

        if (!bStillExists)
        {
            ClearSelection();
        }
    }
}

void UDispatchMapWidget::SpawnMarkerWidgets()
{
    if (!markersCanvas || !markerWidgetClass)
    {
        markerWidgetsByOfferId.Reset();
        return;
    }

    for (auto& Pair : markerWidgetsByOfferId)
    {
        if (Pair.Value)
        {
            Pair.Value->RemoveFromParent();
        }
    }
    markerWidgetsByOfferId.Reset();

    for (const FDispatchMapOfferMarker& M : markers)
    {
        UDispatchMissionOfferMarkerWidget* W = CreateWidget<UDispatchMissionOfferMarkerWidget>(GetOwningPlayer(), markerWidgetClass);
        if (!W)
        {
            continue;
        }

        W->InitializeMarker(M);
        W->OnMarkerClicked.AddDynamic(this, &UDispatchMapWidget::HandleMarkerClicked);

        markersCanvas->AddChild(W);

        if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(W->Slot))
        {
            CanvasSlot->SetAutoSize(true);
            CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        }

        markerWidgetsByOfferId.Add(M.offerId, W);
    }

    bNeedsMarkerReposition = true;
}

void UDispatchMapWidget::UpdateMarkerWidgetPositions()
{
    if (!markersCanvas)
    {
        bNeedsMarkerReposition = false;
        return;
    }

    const FVector2D CanvasSize = markersCanvas->GetCachedGeometry().GetLocalSize();
    if (CanvasSize.X <= 1.f || CanvasSize.Y <= 1.f)
    {
        bNeedsMarkerReposition = true;
        return;
    }

    for (const FDispatchMapOfferMarker& M : markers)
    {
        UDispatchMissionOfferMarkerWidget** Found = markerWidgetsByOfferId.Find(M.offerId);
        if (!Found || !(*Found))
        {
            continue;
        }

        if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>((*Found)->Slot))
        {
            const FVector2D Pos(M.normalizedPos.X * CanvasSize.X, M.normalizedPos.Y * CanvasSize.Y);
            CanvasSlot->SetPosition(Pos);
        }
    }

    bNeedsMarkerReposition = false;
}

void UDispatchMapWidget::ApplyTransform()
{
    if (mapContent)
    {
        mapContent->SetRenderScale(FVector2D(zoom, zoom));
        mapContent->SetRenderTranslation(pan);
    }
    else
    {
        HandleMapTransformChanged(zoom, pan);
    }

    bNeedsMarkerReposition = true;
}

FVector2D UDispatchMapWidget::WorldToNormalized(const FVector& WorldLocation) const
{
    if (!boundsVolume.IsValid())
    {
        return FVector2D(0.5f, 0.5f);
    }

    const FVector Min = boundsVolume->GetWorldMin();
    const FVector Max = boundsVolume->GetWorldMax();

    const float SizeX = FMath::Max(1.f, Max.X - Min.X);
    const float SizeY = FMath::Max(1.f, Max.Y - Min.Y);

    float NX = (WorldLocation.X - Min.X) / SizeX;
    float NY = (WorldLocation.Y - Min.Y) / SizeY;

    NX = FMath::Clamp(NX, 0.f, 1.f);
    NY = FMath::Clamp(NY, 0.f, 1.f);

    if (bInvertY)
    {
        NY = 1.f - NY;
    }

    return FVector2D(NX, NY);
}

void UDispatchMapWidget::UpdateDetailsWidget()
{
    if (!selectedOfferId.IsValid())
    {
        if (detailsWidget)
        {
            detailsWidget->RequestVisible(false);
        }
        return;
    }

    const FDispatchMapOfferMarker* Marker = nullptr;
    for (const FDispatchMapOfferMarker& M : markers)
    {
        if (M.offerId == selectedOfferId)
        {
            Marker = &M;
            break;
        }
    }

    if (!Marker)
    {
        return;
    }

    if (!detailsWidget && detailsWidgetClass)
    {
        detailsWidget = CreateWidget<UDispatchMissionOfferDetailsWidget>(GetOwningPlayer(), detailsWidgetClass);
        if (detailsWidget)
        {
            detailsWidget->OnAcceptRequested.AddDynamic(this, &UDispatchMapWidget::HandleDetailsAccept);
            detailsWidget->OnDeclineRequested.AddDynamic(this, &UDispatchMapWidget::HandleDetailsDecline);

            if (detailsHost)
            {
                detailsHost->AddChild(detailsWidget);
            }
            else
            {
                detailsWidget->AddToViewport(200);
            }
        }
    }

    if (detailsWidget)
    {
        FDispatchMissionOfferSummary Summary;
        Summary.offerId = Marker->offerId;
        Summary.title = Marker->title;
        Summary.missionType = Marker->missionType;
        Summary.monsterChance10 = Marker->monsterChance10;
        Summary.lootChance10 = Marker->lootChance10;
        Summary.complicationChance10 = Marker->complicationChance10;

        detailsWidget->SetOfferSummary(Summary);
    }
}

#pragma endregion INTERNAL

#pragma region CALLBACKS

void UDispatchMapWidget::HandleOfferAdded(const FGuid& OfferId)
{
    RefreshOffers();
}

void UDispatchMapWidget::HandleOfferRemoved(const FGuid& OfferId)
{
    RefreshOffers();
}

void UDispatchMapWidget::HandleMarkerClicked(const FGuid& OfferId)
{
    SelectOffer(OfferId);
}

void UDispatchMapWidget::HandleDetailsAccept(const FGuid& OfferId)
{
    OnOfferAcceptRequested.Broadcast(OfferId);
}

void UDispatchMapWidget::HandleDetailsDecline(const FGuid& OfferId)
{
    OnOfferDeclineRequested.Broadcast(OfferId);
}

#pragma endregion CALLBACKS
