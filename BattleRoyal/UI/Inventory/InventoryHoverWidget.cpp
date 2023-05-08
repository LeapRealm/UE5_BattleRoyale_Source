#include "InventoryHoverWidget.h"

#include "WorldItem.h"
#include "BattleRoyalGameInstance.h"

#include "Components/TextBlock.h"

void UInventoryHoverWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GameInstance = Cast<UBattleRoyalGameInstance>(GetGameInstance());

	FireRateFormatOptions.AlwaysSign = false;
	FireRateFormatOptions.UseGrouping = false;
	FireRateFormatOptions.RoundingMode = ToZero;
	FireRateFormatOptions.MinimumIntegralDigits = 1;
	FireRateFormatOptions.MaximumIntegralDigits = 1;
	FireRateFormatOptions.MinimumFractionalDigits = 0;
	FireRateFormatOptions.MaximumFractionalDigits = 2;
}

void UInventoryHoverWidget::RefreshUI(uint32 ItemID)
{
	const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(ItemID);
	if (ItemInfo == nullptr)
		return;
	
	NameText->SetText(ItemInfo->Name);

	switch (ItemInfo->ItemType)
	{
	case EItemType::Weapon:		ItemTypeText->SetText(FText::FromString(TEXT("무기")));		break;
	case EItemType::Armor:		ItemTypeText->SetText(FText::FromString(TEXT("방어구")));	break;
	case EItemType::Consumable:	ItemTypeText->SetText(FText::FromString(TEXT("소모품")));	break;
	}

	if (ItemInfo->ItemType == EItemType::Weapon)
	{
		FText FireRateText = FText::AsNumber(ItemInfo->FireRate, &FireRateFormatOptions);
		FString String = FString::Printf(TEXT("%s초마다 발사 / %d발"),
			*FireRateText.ToString(), ItemInfo->MaxMagazineAmmoAmount);
		OptionalText->SetText(FText::FromString(String));
	}
	else
	{
		OptionalText->SetText(FText::GetEmpty());
	}
	
	DescriptionText->SetText(ItemInfo->Description);
}
