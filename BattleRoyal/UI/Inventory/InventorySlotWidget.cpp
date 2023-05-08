#include "InventorySlotWidget.h"

#include "InventoryComponent.h"
#include "BattleRoyalCharacter.h"
#include "BattleRoyalGameInstance.h"
#include "InventoryHoverWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventorySlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	GameInstance = Cast<UBattleRoyalGameInstance>(GetGameInstance());

	DefaultIconColor = ItemIcon->ColorAndOpacity;
	DefaultBackgroundColor = Background->GetBrushColor();
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	HoverWidget->RefreshUI(ItemObject.ItemID);
	HoverWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	
	ItemIcon->SetColorAndOpacity(DefaultIconColor - FLinearColor(0.2f, 0.2f, 0.2f, 0.f));
	Background->SetBrushColor(DefaultBackgroundColor - FLinearColor(0.03f, 0.03f, 0.03f, 0.f));
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	HoverWidget->SetVisibility(ESlateVisibility::Hidden);
	
	ItemIcon->SetColorAndOpacity(DefaultIconColor);
	Background->SetBrushColor(DefaultBackgroundColor);
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (ItemObject.ItemID == 0)
		return Reply;

	ABattleRoyalCharacter* CharacterOwner = Cast<ABattleRoyalCharacter>(GetOwningPlayerPawn());
	if (IsValid(CharacterOwner) == false)
		return Reply;
		
	const FItemBaseInfo* ItemBaseInfo = GameInstance->GetItemBaseInfo(ItemObject.ItemID);
	if (ItemBaseInfo == nullptr)
		return Reply;
	
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		if (InventorySlotType == EInventorySlotType::Near)
		{
			switch (ItemBaseInfo->ItemType)
			{
			case EItemType::Weapon:
				CharacterOwner->GetInventoryComponent()->Server_RequestSetWeapon(SlotID, ItemObject);
				break;
			case EItemType::Armor:
				CharacterOwner->GetInventoryComponent()->Server_RequestSetArmor(SlotID, ItemObject);
				break;
			case EItemType::Consumable:
				CharacterOwner->GetInventoryComponent()->Server_RequestMoveConsumable(
					EInventorySlotType::Near, SlotID, ItemObject);
				break;
			}
		}
		else if (InventorySlotType == EInventorySlotType::Inventory)
		{
			if (ItemBaseInfo->ItemType != EItemType::Consumable)
				return Reply;
				
			CharacterOwner->GetInventoryComponent()->Server_RequestMoveConsumable(
					EInventorySlotType::Inventory, SlotID, ItemObject);
		}

		return FReply::Handled();
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (InventorySlotType == EInventorySlotType::Inventory && ItemBaseInfo->ConsumableType == EConsumableType::HealthKit)
			CharacterOwner->UseHealthKit();
	}

	return Reply;
}

void UInventorySlotWidget::Init(uint32 NewSlotID, EInventorySlotType NewInvenSlotType, UInventoryHoverWidget* NewHoverWidget)
{
	SlotID = NewSlotID;
	InventorySlotType = NewInvenSlotType;
	HoverWidget = NewHoverWidget;
}

void UInventorySlotWidget::RefreshUI(const FItemObject& NewItemObject)
{
	ItemObject = NewItemObject;
	
	const FItemBaseInfo* ItemBaseInfo = GameInstance->GetItemBaseInfo(ItemObject.ItemID);
	if (ItemBaseInfo == nullptr)
		return;
	
	ItemIcon->SetBrushFromTexture(ItemBaseInfo->Icon);
	ItemNameText->SetText(ItemBaseInfo->Name);

	if (ItemObject.Amount > 1)
		ItemAmountText->SetText(FText::AsNumber(ItemObject.Amount));
	else
		ItemAmountText->SetText(FText::GetEmpty());
}
