#include "InventoryWidget.h"

#include "InventoryComponent.h"
#include "BattleRoyalCharacter.h"
#include "ArmorSlotWidget.h"
#include "InventoryHoverWidget.h"
#include "InventorySlotWidget.h"
#include "WeaponSlotWidget.h"
#include "WorldItem.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_InventorySlot(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BattleRoyal/UI/Widgets/Inventory/WBP_InventorySlot.WBP_InventorySlot_C'"));
	if (WBP_InventorySlot.Succeeded())
		InventorySlotWidgetClass = WBP_InventorySlot.Class;
}

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	bIsFocusable = true;

	// Armor
	ArmorSlots[0] = HelmetSlot;
	ArmorSlots[1] = ShirtSlot;
	ArmorSlots[2] = VestSlot;
	ArmorSlots[3] = BackpackSlot;
	ArmorSlots[4] = PantsSlot;
	ArmorSlots[5] = BootsSlot;

	for (int32 i = 0; i < ArmorSlotCount; i++)
		ArmorSlots[i]->Init(HoverWidget, static_cast<EArmorType>(i));

	// Weapon
	WeaponSlots[0] = PrimarySlot;
	WeaponSlots[1] = SecondarySlot;
	WeaponSlots[2] = TertiarySlot;

	for (int32 i = 0; i < WeaponSlotCount; i++)
		WeaponSlots[i]->Init(HoverWidget, static_cast<EWeaponSlotType>(i));
	
	// Hover
	HoverWidget->SetVisibility(ESlateVisibility::Hidden);
	HoverSlot = Cast<UCanvasPanelSlot>(HoverWidget->Slot);
}

FReply UInventoryWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	FVector2D LocalMousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D Space = FVector2D(20.f, 20.f);
	FVector2D ClampPos = LocalMousePos + Space + HoverWidget->GetCachedGeometry().Size;
	FVector2D HoverWidgetPos = LocalMousePos + Space;
	
	if (ClampPos.X > InGeometry.Size.X)
		HoverWidgetPos.X = LocalMousePos.X - (Space.X + HoverWidget->GetCachedGeometry().Size.X);

	if (ClampPos.Y > InGeometry.Size.Y)
		HoverWidgetPos.Y = LocalMousePos.Y - (Space.Y + HoverWidget->GetCachedGeometry().Size.Y);

	HoverSlot->SetPosition(HoverWidgetPos);
	
	return Reply;
}

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	
	if (InKeyEvent.IsRepeat() == false && InKeyEvent.GetKey() == EKeys::Tab && GetVisibility() == ESlateVisibility::Visible)
	{
		ABattleRoyalCharacter* CharacterOwner = Cast<ABattleRoyalCharacter>(GetOwningPlayerPawn());
		if (IsValid(CharacterOwner) == false)
			return Reply;
		
		CharacterOwner->GetInventoryComponent()->Server_UnlockWorldItems();
		SetVisibility(ESlateVisibility::Hidden);
		
		if (APlayerController* PlayerController = GetOwningPlayer())
		{
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController, true);
			PlayerController->SetShowMouseCursor(false);
		}
		
		return FReply::Handled();
	}
    
	return Reply;
}

void UInventoryWidget::RefreshNicknameUI(const FText& Nickname)
{
	NicknameText->SetText(Nickname);
}

void UInventoryWidget::RefreshSlotUI(const TArray<FItemObject>& NearItems, const TArray<FItemObject>& InventoryItems)
{
	RefreshNearSlotUI(NearItems);
	RefreshInventorySlotUI(InventoryItems);
}

void UInventoryWidget::RefreshNearSlotUI(const TArray<FItemObject>& NearItems)
{
	HoverWidget->SetVisibility(ESlateVisibility::Hidden);
	
	NearScrollBox->ClearChildren();
	for (int32 i = 0; i < NearItems.Num(); i++)
	{
		UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), InventorySlotWidgetClass);
		SlotWidget->Init(i, EInventorySlotType::Near, HoverWidget);
		SlotWidget->RefreshUI(NearItems[i]);
		NearScrollBox->AddChild(SlotWidget);
	}
}

void UInventoryWidget::RefreshInventorySlotUI(const TArray<FItemObject>& InventoryItems)
{
	HoverWidget->SetVisibility(ESlateVisibility::Hidden);
	
	InventoryScrollBox->ClearChildren();

	for (int32 i = 0; i < InventoryItems.Num(); i++)
	{
		UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), InventorySlotWidgetClass);
		SlotWidget->Init(i, EInventorySlotType::Inventory, HoverWidget);
		SlotWidget->RefreshUI(InventoryItems[i]);
		InventoryScrollBox->AddChild(SlotWidget);
	}
}

void UInventoryWidget::RefreshArmorUI()
{
	ABattleRoyalCharacter* CharacterOwner = Cast<ABattleRoyalCharacter>(GetOwningPlayerPawn());
	if (IsValid(CharacterOwner) == false)
		return;
	
	TArray<FItemObject>& ArmorItems = CharacterOwner->GetInventoryComponent()->GetArmorSlots();
	for (int32 i = 0; i < ArmorSlotCount; i++)
		ArmorSlots[i]->RefreshUI(ArmorItems[i]);
}

void UInventoryWidget::RefreshWeaponUI()
{
	ABattleRoyalCharacter* CharacterOwner = Cast<ABattleRoyalCharacter>(GetOwningPlayerPawn());
	if (IsValid(CharacterOwner) == false)
		return;
	
	TArray<FItemObject>& WeaponItems = CharacterOwner->GetInventoryComponent()->GetWeaponSlots();
	for (int32 i = 0; i < WeaponSlotCount; i++)
		WeaponSlots[i]->RefreshUI(WeaponItems[i]);
}
