#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ElectricCircleWidget.generated.h"

class UCanvasPanelSlot;
class UImage;

UCLASS()
class BATTLEROYAL_API UElectricCircleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ElectricFieldCircle;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMaterialInstanceDynamic> CircleMaterial;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> CircleWidgetSlot;

private:
	UPROPERTY(EditAnywhere)
	float Thickness = 400.f;
};
