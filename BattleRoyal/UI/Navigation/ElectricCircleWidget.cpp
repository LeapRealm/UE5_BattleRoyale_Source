#include "ElectricCircleWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"

void UElectricCircleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CircleMaterial = ElectricFieldCircle->GetDynamicMaterial();
	CircleWidgetSlot = Cast<UCanvasPanelSlot>(Slot);
}

void UElectricCircleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	float OuterCircle;
	CircleMaterial->GetScalarParameterValue(TEXT("OuterCircle"), OuterCircle);
	
	float InnerCircle = OuterCircle - (Thickness / CircleWidgetSlot->GetSize().X * 0.01);
	CircleMaterial->SetScalarParameterValue(TEXT("InnerCircle"), InnerCircle);
}
