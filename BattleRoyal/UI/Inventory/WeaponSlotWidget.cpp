#include "WeaponSlotWidget.h"

#include "BattleRoyalCharacter.h"
#include "BattleRoyalGameInstance.h"
#include "InventoryHoverWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UWeaponSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GameInstance = Cast<UBattleRoyalGameInstance>(GetGameInstance());

	DefaultIconColor = ItemIcon->ColorAndOpacity;
	DefaultBackgroundColor = Background->GetBrushColor();
}

void UWeaponSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (ItemObject.ItemID != 0)
	{
		HoverWidget->RefreshUI(ItemObject.ItemID);
		HoverWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	
	ItemIcon->SetColorAndOpacity(DefaultIconColor - FLinearColor(0.2f, 0.2f, 0.2f, 0.f));
	Background->SetBrushColor(DefaultBackgroundColor - FLinearColor(0.03f, 0.03f, 0.03f, 0.f));
}

void UWeaponSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	HoverWidget->SetVisibility(ESlateVisibility::Hidden);
	
	ItemIcon->SetColorAndOpacity(DefaultIconColor);
	Background->SetBrushColor(DefaultBackgroundColor);
}

FReply UWeaponSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		if (ItemObject.ItemID == 0)
			return Reply;

		ABattleRoyalCharacter* CharacterOwner = Cast<ABattleRoyalCharacter>(GetOwningPlayerPawn());
		if (IsValid(CharacterOwner))
			CharacterOwner->GetInventoryComponent()->Server_RequestRemoveWeapon(WeaponSlotType, ItemObject);

		return FReply::Handled();
	}

	return Reply;
}

void UWeaponSlotWidget::Init(UInventoryHoverWidget* NewHoverWidget, EWeaponSlotType NewWeaponSlotType)
{
	HoverWidget = NewHoverWidget;
	WeaponSlotType = NewWeaponSlotType;

	NumberText->SetText(FText::FromString(FString::Printf(TEXT("%d"), WeaponSlotType + 1)));
}

void UWeaponSlotWidget::RefreshUI(const FItemObject& NewItemObject)
{
	ItemObject = NewItemObject;

	if (ItemObject.ItemID == 0)
	{
		BaseIcon->SetRenderOpacity(0.5f);
		ItemIcon->SetRenderOpacity(0.f);

		NameText->SetText(FText::GetEmpty());
		AmmoText->SetText(FText::GetEmpty());
	}
	else
	{
		BaseIcon->SetRenderOpacity(0.f);
		ItemIcon->SetRenderOpacity(1.f);
		
		const FItemBaseInfo* ItemBaseInfo = GameInstance->GetItemBaseInfo(ItemObject.ItemID);
		ItemIcon->SetBrushFromTexture(ItemBaseInfo->Icon);

		NameText->SetText(ItemBaseInfo->Name);

		FString AmmoString = FString::Printf(TEXT("%d / %d"), ItemObject.MagazineAmmoAmount, ItemBaseInfo->MaxMagazineAmmoAmount);
		AmmoText->SetText(FText::FromString(AmmoString));
	}
}
