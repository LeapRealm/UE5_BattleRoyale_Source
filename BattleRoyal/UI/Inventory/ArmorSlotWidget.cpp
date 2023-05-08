#include "ArmorSlotWidget.h"

#include "InventoryComponent.h"
#include "BattleRoyalCharacter.h"
#include "BattleRoyalGameInstance.h"
#include "InventoryHoverWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"

UArmorSlotWidget::UArmorSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> T_HelmetIcon(TEXT("/Script/Engine.Texture2D'/Game/BattleRoyal/UI/Textures/T_Helmet.T_Helmet'"));
	if (T_HelmetIcon.Succeeded())
		HelmetIcon = T_HelmetIcon.Object;
	
	static ConstructorHelpers::FObjectFinder<UTexture2D> T_ShirtIcon(TEXT("/Script/Engine.Texture2D'/Game/BattleRoyal/UI/Textures/T_ShirtIcon.T_ShirtIcon'"));
	if (T_ShirtIcon.Succeeded())
		ShirtIcon = T_ShirtIcon.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> T_VestIcon(TEXT("/Script/Engine.Texture2D'/Game/BattleRoyal/UI/Textures/T_VestIcon.T_VestIcon'"));
	if (T_VestIcon.Succeeded())
		VestIcon = T_VestIcon.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> T_BackpackIcon(TEXT("/Script/Engine.Texture2D'/Game/BattleRoyal/UI/Textures/T_BackpackIcon.T_BackpackIcon'"));
	if (T_BackpackIcon.Succeeded())
		BackpackIcon = T_BackpackIcon.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> T_PantsIcon(TEXT("/Script/Engine.Texture2D'/Game/BattleRoyal/UI/Textures/T_PantsIcon.T_PantsIcon'"));
	if (T_PantsIcon.Succeeded())
		PantsIcon = T_PantsIcon.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> T_BootsIcon(TEXT("/Script/Engine.Texture2D'/Game/BattleRoyal/UI/Textures/T_BootsIcon.T_BootsIcon'"));
	if (T_BootsIcon.Succeeded())
		BootsIcon = T_BootsIcon.Object;
}

void UArmorSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GameInstance = Cast<UBattleRoyalGameInstance>(GetGameInstance());
	
	DefaultIconColor = ItemIcon->ColorAndOpacity;
	DefaultBackgroundColor = Background->GetBrushColor();
}

void UArmorSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

void UArmorSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	HoverWidget->SetVisibility(ESlateVisibility::Hidden);
	
	ItemIcon->SetColorAndOpacity(DefaultIconColor);
	Background->SetBrushColor(DefaultBackgroundColor);
}

FReply UArmorSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		if (ItemObject.ItemID == 0)
			return Reply;

		ABattleRoyalCharacter* CharacterOwner = Cast<ABattleRoyalCharacter>(GetOwningPlayerPawn());
		if (IsValid(CharacterOwner))
			CharacterOwner->GetInventoryComponent()->Server_RequestRemoveArmor(ArmorSlotType, ItemObject);
		
		return FReply::Handled();
	}

	return Reply;
}

void UArmorSlotWidget::Init(UInventoryHoverWidget* NewHoverWidget, EArmorType NewArmorSlotType)
{
	HoverWidget = NewHoverWidget;
	ArmorSlotType = NewArmorSlotType;

	switch (ArmorSlotType)
	{
	case EArmorType::Helmet:	BaseIcon->SetBrushFromTexture(HelmetIcon);		break;
	case EArmorType::Shirt:		BaseIcon->SetBrushFromTexture(ShirtIcon);		break;
	case EArmorType::Vest:		BaseIcon->SetBrushFromTexture(VestIcon);		break;
	case EArmorType::Backpack:	BaseIcon->SetBrushFromTexture(BackpackIcon);	break;
	case EArmorType::Pants:		BaseIcon->SetBrushFromTexture(PantsIcon);		break;
	case EArmorType::Boots:		BaseIcon->SetBrushFromTexture(BootsIcon);		break;
	}
}

void UArmorSlotWidget::RefreshUI(const FItemObject& NewItemObject)
{
	ItemObject = NewItemObject;

	if (ItemObject.ItemID == 0)
	{
		BaseIcon->SetRenderOpacity(0.5f);
		ItemIcon->SetRenderOpacity(0.f);
	}
	else
	{
		BaseIcon->SetRenderOpacity(0.f);
		ItemIcon->SetRenderOpacity(1.f);

		if (GameInstance)
		{
			const FItemBaseInfo* ItemBaseInfo = GameInstance->GetItemBaseInfo(ItemObject.ItemID);
			if (ItemBaseInfo)
				ItemIcon->SetBrushFromTexture(ItemBaseInfo->Icon);
		}
	}
}
