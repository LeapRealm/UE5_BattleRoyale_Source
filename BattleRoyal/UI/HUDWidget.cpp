#include "HUDWidget.h"

#include "OptionWidget.h"
#include "InventoryWidget.h"
#include "WorldMapWidget.h"
#include "ReticleWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	WorldMapWidget->SetMiniMapWidget(MiniMapWidget);
	WorldMapWidget->SetCompassBarWidget(CompassBarWidget);
	
	WorldMapWidget->SetVisibility(ESlateVisibility::Hidden);
	InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	OptionWidget->SetVisibility(ESlateVisibility::Hidden);

	ShowUnarmedReticle();
}

void UHUDWidget::RefreshReticleUI(float SpreadFactor)
{
	if (ReticlePistolWidget->GetVisibility() == ESlateVisibility::Visible)
		ReticlePistolWidget->RefreshUI(SpreadFactor);
	else if (ReticleRifleWidget->GetVisibility() == ESlateVisibility::Visible)
		ReticleRifleWidget->RefreshUI(SpreadFactor);
	else if (ReticleShotgunWidget->GetVisibility() == ESlateVisibility::Visible)
		ReticleShotgunWidget->RefreshUI(SpreadFactor);
}

void UHUDWidget::ShowUnarmedReticle()
{
	ReticleUnarmedWidget->SetVisibility(ESlateVisibility::Visible);
	ReticlePistolWidget->SetVisibility(ESlateVisibility::Hidden);
	ReticleRifleWidget->SetVisibility(ESlateVisibility::Hidden);
	ReticleShotgunWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UHUDWidget::ShowPistolReticle()
{
	ReticleUnarmedWidget->SetVisibility(ESlateVisibility::Hidden);
	ReticlePistolWidget->SetVisibility(ESlateVisibility::Visible);
	ReticleRifleWidget->SetVisibility(ESlateVisibility::Hidden);
	ReticleShotgunWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UHUDWidget::ShowRifleReticle()
{
	ReticleUnarmedWidget->SetVisibility(ESlateVisibility::Hidden);
	ReticlePistolWidget->SetVisibility(ESlateVisibility::Hidden);
	ReticleRifleWidget->SetVisibility(ESlateVisibility::Visible);
	ReticleShotgunWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UHUDWidget::ShowShotgunReticle()
{
	ReticleUnarmedWidget->SetVisibility(ESlateVisibility::Hidden);
	ReticlePistolWidget->SetVisibility(ESlateVisibility::Hidden);
	ReticleRifleWidget->SetVisibility(ESlateVisibility::Hidden);
	ReticleShotgunWidget->SetVisibility(ESlateVisibility::Visible);
}

void UHUDWidget::SetReticleVisible()
{
	ReticleUnarmedWidget->SetRenderOpacity(1.f);
	ReticlePistolWidget->SetRenderOpacity(1.f);
	ReticleRifleWidget->SetRenderOpacity(1.f);
	ReticleShotgunWidget->SetRenderOpacity(1.f);
}

void UHUDWidget::SetReticleInvisible()
{
	ReticleUnarmedWidget->SetRenderOpacity(0.f);
	ReticlePistolWidget->SetRenderOpacity(0.f);
	ReticleRifleWidget->SetRenderOpacity(0.f);
	ReticleShotgunWidget->SetRenderOpacity(0.f);
}
