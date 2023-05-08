#include "ReticleWidget.h"

#include "Components/SizeBox.h"

void UReticleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitialCrosshairSize = FVector2D(CrosshairSizeBox->GetWidthOverride(), CrosshairSizeBox->GetHeightOverride());
	InitialCircleCrosshairSize = FVector2D(CircleCrosshairSizeBox->GetWidthOverride(), CircleCrosshairSizeBox->GetHeightOverride());
}

void UReticleWidget::RefreshUI(float SpreadFactor)
{
	FVector2D Size = InitialCrosshairSize * SpreadFactor;
	CrosshairSizeBox->SetWidthOverride(Size.X);
	CrosshairSizeBox->SetHeightOverride(Size.Y);
	
	Size = InitialCircleCrosshairSize * SpreadFactor;
	CircleCrosshairSizeBox->SetWidthOverride(Size.X);
	CircleCrosshairSizeBox->SetHeightOverride(Size.Y);
}

void UReticleWidget::PlayHitMarker()
{
	PlayAnimation(HitMarkerAnim);
}

void UReticleWidget::PlayKillMarker()
{
	PlayAnimation(KillMarkerAnim);
}
