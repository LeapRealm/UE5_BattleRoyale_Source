#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "WorldItem.h"
#include "InventoryWidget.generated.h"

class UTextBlock;
class UWeaponSlotWidget;
class UCanvasPanelSlot;
class UInventoryHoverWidget;
class UImage;
class UScrollBox;
class UArmorSlotWidget;
struct FItemObject;

UCLASS()
class BATTLEROYAL_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UInventoryWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
	void RefreshNicknameUI(const FText& Nickname);
	void RefreshSlotUI(const TArray<FItemObject>& NearItems, const TArray<FItemObject>& InventoryItems);
	void RefreshNearSlotUI(const TArray<FItemObject>& NearItems);
	void RefreshInventorySlotUI(const TArray<FItemObject>& InventoryItems);
	void RefreshArmorUI();
	void RefreshWeaponUI();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> HoverSlot;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> InventorySlotWidgetClass;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryHoverWidget> HoverWidget;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> NearScrollBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> InventoryScrollBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> NicknameText;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UArmorSlotWidget> HelmetSlot;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UArmorSlotWidget> ShirtSlot;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UArmorSlotWidget> VestSlot;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UArmorSlotWidget> BackpackSlot;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UArmorSlotWidget> PantsSlot;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UArmorSlotWidget> BootsSlot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArmorSlotWidget> ArmorSlots[ArmorSlotCount];

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWeaponSlotWidget> PrimarySlot;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWeaponSlotWidget> SecondarySlot;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWeaponSlotWidget> TertiarySlot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWeaponSlotWidget> WeaponSlots[WeaponSlotCount];
};
