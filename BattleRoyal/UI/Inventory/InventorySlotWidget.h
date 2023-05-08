#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldItem.h"
#include "InventorySlotWidget.generated.h"

UENUM()
enum class EInventorySlotType : uint8
{
	Near,
	Inventory,

	None
};

class UImage;
class UBorder;
class UTextBlock;
class UInventoryHoverWidget;
class UBattleRoyalGameInstance;

UCLASS()
class BATTLEROYAL_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
public:
	void Init(uint32 NewSlotID, EInventorySlotType NewInvenSlotType, UInventoryHoverWidget* NewHoverWidget);
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
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Background;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ItemIcon;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemAmountText;

private:
	UPROPERTY(VisibleAnywhere)
	uint32 SlotID;

	UPROPERTY(VisibleAnywhere)
	EInventorySlotType InventorySlotType = EInventorySlotType::None;

	UPROPERTY(VisibleAnywhere)
	FItemObject ItemObject;
};
