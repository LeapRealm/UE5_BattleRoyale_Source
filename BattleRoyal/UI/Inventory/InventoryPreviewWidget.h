#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPreviewWidget.generated.h"

class UImage;
class ABattleRoyalPlayerController;

UCLASS()
class BATTLEROYAL_API UInventoryPreviewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PreviewImage;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ABattleRoyalPlayerController> ControllerOwner;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTextureRenderTarget2D> RenderTarget2D;

private:
	UPROPERTY(VisibleAnywhere)
	uint8 bIsDragging:1;
};
