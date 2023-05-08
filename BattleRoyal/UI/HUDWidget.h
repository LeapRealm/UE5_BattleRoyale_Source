#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class UOptionWidget;
class UNoticeWidget;
class UReticleWidget;
class UInventoryWidget;
class UHealthBarWidget;
class UMiniMapWidget;
class UWorldMapWidget;
class UCompassBarWidget;

UCLASS()
class BATTLEROYAL_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

public:
	void RefreshReticleUI(float SpreadFactor);
	
	void ShowUnarmedReticle();
	void ShowPistolReticle();
	void ShowRifleReticle();
	void ShowShotgunReticle();

	void SetReticleVisible();
	void SetReticleInvisible();
	
public:
	// Navigation
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UMiniMapWidget> MiniMapWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWorldMapWidget> WorldMapWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCompassBarWidget> CompassBarWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryWidget> InventoryWidget;

	// Reticle
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UReticleWidget> ReticleUnarmedWidget;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UReticleWidget> ReticlePistolWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UReticleWidget> ReticleRifleWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UReticleWidget> ReticleShotgunWidget;

	// HUD
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHealthBarWidget> HealthBarWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UNoticeWidget> NoticeWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionWidget> OptionWidget;
};
