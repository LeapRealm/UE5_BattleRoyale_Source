#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "WorldItem.h"
#include "WeaponSlotWidget.generated.h"

class UTextBlock;
class UImage;
class UBorder;
class UInventoryHoverWidget;
class UBattleRoyalGameInstance;

UCLASS()
class BATTLEROYAL_API UWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
public:
	void Init(UInventoryHoverWidget* NewHoverWidget, EWeaponSlotType NewWeaponSlotType);
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
	TObjectPtr<UImage> BaseIcon;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ItemIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> NameText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> NumberText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> AmmoText;

private:
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<EWeaponSlotType> WeaponSlotType = EWeaponSlotType::Primary;

	UPROPERTY(VisibleAnywhere)
	FItemObject ItemObject;
};
