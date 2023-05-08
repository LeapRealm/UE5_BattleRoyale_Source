#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReticleWidget.generated.h"

class UImage;
class USizeBox;

UCLASS()
class BATTLEROYAL_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

public:
	void RefreshUI(float SpreadFactor);
	
	void PlayHitMarker();
	void PlayKillMarker();

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> CrosshairSizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> CircleCrosshairSizeBox;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> HitMarkerAnim;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> KillMarkerAnim;
	
	FVector2D InitialCrosshairSize;
	FVector2D InitialCircleCrosshairSize;
};
