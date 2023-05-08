#include "MiniMapWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

void UMiniMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetSlot = Cast<UCanvasPanelSlot>(Slot);
	PinIconSlot = Cast<UCanvasPanelSlot>(PinIcon->Slot);
	ArrowIconSlot = Cast<UCanvasPanelSlot>(ArrowIcon->Slot);
	PlayerIconSlot = Cast<UCanvasPanelSlot>(PlayerIcon->Slot);
	MiniMapPanelSlot = Cast<UCanvasPanelSlot>(MiniMapPanel->Slot);
	InitialMiniMapSize = MiniMapPanelSlot->GetSize();
	
	PinIcon->SetVisibility(ESlateVisibility::Hidden);
	ArrowIcon->SetVisibility(ESlateVisibility::Hidden);
}

void UMiniMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Refresh MiniMap Panel Position
	APawn* Pawn = GetOwningPlayerPawn();
	if (IsValid(Pawn))
	{
		FVector PawnLocation = Pawn->GetActorLocation();
		MiniMapPanelSlot->SetPosition(PawnWorldPosToMiniMapPanelPos(FVector2D(PawnLocation.X, PawnLocation.Y)));
	}

	// Refresh Arrow Icon Rotation
	FVector Start = FVector(PlayerIconSlot->GetPosition(), 0);
	FVector Target = FVector(MiniMapPanelSlot->GetPosition() + PinIconSlot->GetPosition(), 0.f);
	FRotator LookRotator = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	ArrowIcon->SetRenderTransformAngle(LookRotator.Yaw);

	// Refresh Arrow Icon Position & Visibility
	FVector2D PinIconPanelPos = MiniMapPanelSlot->GetPosition() + PinIconSlot->GetPosition();
	FVector2D WidgetHalfSize = WidgetSlot->GetSize() / 2.f;
	
	bool bVisible = PinIcon->GetVisibility() == ESlateVisibility::Visible;
	bool bOutOfMiniMap = PinIconPanelPos.X <= -WidgetHalfSize.X || PinIconPanelPos.X >= WidgetHalfSize.X ||
		                  PinIconPanelPos.Y <= -WidgetHalfSize.Y || PinIconPanelPos.Y >= WidgetHalfSize.Y;
	if (bVisible && bOutOfMiniMap)
	{
		FVector2D LimitPos = WidgetHalfSize - PinIconSlot->GetSize();
		float x = FMath::Clamp(PinIconPanelPos.X, -LimitPos.X, LimitPos.X);
		float y = FMath::Clamp(PinIconPanelPos.Y, -LimitPos.Y, LimitPos.Y);
		ArrowIconSlot->SetPosition(FVector2D(x, y));

		ArrowIcon->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ArrowIcon->SetVisibility(ESlateVisibility::Hidden);
	}

	// Refresh Player Panel Rotation
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		if (APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager)
			PlayerPanel->SetRenderTransformAngle(CameraManager->GetCameraRotation().Yaw);
	}
}

void UMiniMapWidget::ZoomIn()
{
	CurrentMiniMapZoom = FMath::Clamp(CurrentMiniMapZoom + DeltaMiniMapZoom, MinMiniMapZoom, MaxMiniMapZoom);
	RefreshZoom();
}

void UMiniMapWidget::ZoomOut()
{
	CurrentMiniMapZoom = FMath::Clamp(CurrentMiniMapZoom - DeltaMiniMapZoom, MinMiniMapZoom, MaxMiniMapZoom);
	RefreshZoom();
}

void UMiniMapWidget::RefreshZoom()
{
	MiniMapPanelSlot->SetSize(InitialMiniMapSize * CurrentMiniMapZoom);
	PinIconSlot->SetPosition(InitialIconPos * CurrentMiniMapZoom);
}

void UMiniMapWidget::ShowPinIcon(const FVector& PinWorldPos)
{
	PinIcon->SetVisibility(ESlateVisibility::Visible);
	InitialIconPos = PinWorldPosToMiniMapInitialPos(FVector2D(PinWorldPos.X, PinWorldPos.Y));
	RefreshZoom();
}

void UMiniMapWidget::HidePinIcon()
{
	PinIcon->SetVisibility(ESlateVisibility::Hidden);
}

FVector2D UMiniMapWidget::PawnWorldPosToMiniMapPanelPos(const FVector2D& PawnWorldPos)
{
	float x = FMath::GetMappedRangeValueUnclamped(
		FVector2D(WorldFirstPos.Y, WorldSecondPos.Y),
		FVector2D(WidgetFirstPos.X, WidgetSecondPos.X),
		PawnWorldPos.Y);

	float y = FMath::GetMappedRangeValueUnclamped(
		FVector2D(WorldFirstPos.X, WorldSecondPos.X),
		FVector2D(WidgetFirstPos.Y, WidgetSecondPos.Y),
		PawnWorldPos.X);
	
	return FVector2D(x, y) * CurrentMiniMapZoom;
}

FVector2D UMiniMapWidget::PinWorldPosToMiniMapInitialPos(const FVector2D& PinWorldPos)
{
	float x = FMath::GetMappedRangeValueUnclamped(
		FVector2D(WorldFirstPos.Y, WorldSecondPos.Y),
		FVector2D(-WidgetFirstPos.X, -WidgetSecondPos.X),
		PinWorldPos.Y);

	float y = FMath::GetMappedRangeValueUnclamped(
		FVector2D(WorldFirstPos.X, WorldSecondPos.X),
		FVector2D(-WidgetFirstPos.Y, -WidgetSecondPos.Y),
		PinWorldPos.X);
	
	return FVector2D(x, y);
}
