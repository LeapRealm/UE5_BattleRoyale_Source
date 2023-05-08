#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldItem.h"
#include "ArmorSlotWidget.generated.h"

class UImage;
class UBorder;
class UInventoryHoverWidget;
class UBattleRoyalGameInstance;

UCLASS()
class BATTLEROYAL_API UArmorSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UArmorSlotWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeOnInitialized() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
public:
	void Init(UInventoryHoverWidget* NewHoverWidget, EArmorType NewArmorSlotType);
	void RefreshUI(const FItemObject& NewItemObject);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBattleRoyalGameInstance> GameInstance;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInventoryHoverWidget> HoverWidget;

	UPROPERTY(VisibleAnywhere)
	FLinearColor DefaultIconColor;

	UPROPERTY(VisibleAnywhere)
	FLinearColor DefaultBackgroundColor;
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> HelmetIcon;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ShirtIcon;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> VestIcon;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> BackpackIcon;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> PantsIcon;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> BootsIcon;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder >Background;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> BaseIcon;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ItemIcon;

private:
	UPROPERTY(VisibleAnywhere)
	EArmorType ArmorSlotType = EArmorType::None;

	UPROPERTY(VisibleAnywhere)
	FItemObject ItemObject;
};
