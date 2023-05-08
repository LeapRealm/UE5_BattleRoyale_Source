#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniMapWidget.generated.h"

class UImage;
class UCanvasPanel;
class UCanvasPanelSlot;

UCLASS()
class BATTLEROYAL_API UMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void ZoomIn();
	void ZoomOut();
	
	void ShowPinIcon(const FVector& PinWorldPos);
	void HidePinIcon();
	
private:
	void RefreshZoom();
	
	FVector2D PawnWorldPosToMiniMapPanelPos(const FVector2D& PawnWorldPos);
	FVector2D PinWorldPosToMiniMapInitialPos(const FVector2D& PinWorldPos);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> WidgetSlot;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> PinIconSlot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> ArrowIconSlot;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> PlayerIconSlot;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> MiniMapPanelSlot;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ArrowIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> MainPanel;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> MiniMapPanel;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PinIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PlayerIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> PlayerPanel;

private:
	float CurrentMiniMapZoom = 1.f;
	float MinMiniMapZoom = 0.5f;
	float MaxMiniMapZoom = 3.f;
	float DeltaMiniMapZoom = 0.5f;

	FVector2D InitialMiniMapSize;
	FVector2D InitialIconPos;

private:
	FVector2D WorldFirstPos = FVector2D(79270.f, -149770.f);
	FVector2D WorldSecondPos = FVector2D(272410.f, -262810.f);
	
	FVector2D WidgetFirstPos = FVector2D(-260.8f, -608.3f);
	FVector2D WidgetSecondPos = FVector2D(303.5f, 352.1f);
};
