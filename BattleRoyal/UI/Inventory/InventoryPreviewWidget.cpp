#include "InventoryPreviewWidget.h"

#include "BattleRoyalPlayerController.h"
#include "InventoryPreview.h"

#include "Components/Image.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetRenderingLibrary.h"

void UInventoryPreviewWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ControllerOwner = Cast<ABattleRoyalPlayerController>(GetOwningPlayer());
	
	RenderTarget2D = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 960, 1800);

	if (ControllerOwner)
		ControllerOwner->GetInventoryPreview()->CaptureComponent->TextureTarget = RenderTarget2D;

	PreviewImage->GetDynamicMaterial()->SetTextureParameterValue(TEXT("RenderTarget"), RenderTarget2D);
}

FReply UInventoryPreviewWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	bIsDragging = true;
	return FReply::Handled();
}

FReply UInventoryPreviewWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (bIsDragging)
	{
		FVector2D Delta = InMouseEvent.GetCursorDelta();

		if (ControllerOwner)
			ControllerOwner->GetInventoryPreview()->HeadMeshComponent->AddRelativeRotation(FRotator(0.f, -Delta.X, 0.f));
		
		return FReply::Handled();
	}

	return Reply;
}

FReply UInventoryPreviewWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	bIsDragging = false;
	return FReply::Handled();
}

void UInventoryPreviewWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	bIsDragging = false;
}
