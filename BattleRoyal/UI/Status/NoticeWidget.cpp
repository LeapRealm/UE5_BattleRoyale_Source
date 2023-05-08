#include "NoticeWidget.h"

#include "InventoryComponent.h"
#include "BattleRoyalCharacter.h"
#include "BattleRoyalGameInstance.h"
#include "BattleRoyalGameState.h"
#include "BattleRoyalPlayerController.h"

#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

UNoticeWidget::UNoticeWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> T_UnarmedIcon(TEXT("/Script/Engine.Texture2D'/Game/BattleRoyal/UI/Textures/T_UnarmedIcon.T_UnarmedIcon'"));
	if (T_UnarmedIcon.Succeeded())
		UnarmedIcon = T_UnarmedIcon.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> T_PistolIcon(TEXT("/Script/Engine.Texture2D'/Game/BattleRoyal/UI/Textures/T_PistolIcon.T_PistolIcon'"));
	if (T_PistolIcon.Succeeded())
		PistolIcon = T_PistolIcon.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> T_RifleIcon(TEXT("/Script/Engine.Texture2D'/Game/BattleRoyal/UI/Textures/T_RifleIcon.T_RifleIcon'"));
	if (T_RifleIcon.Succeeded())
		RifleIcon = T_RifleIcon.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> T_ShotgunIcon(TEXT("/Script/Engine.Texture2D'/Game/BattleRoyal/UI/Textures/T_ShotgunIcon.T_ShotgunIcon'"));
	if (T_ShotgunIcon.Succeeded())
		ShotgunIcon = T_ShotgunIcon.Object;
}

void UNoticeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HealthKitPanel->SetVisibility(ESlateVisibility::Hidden);
	InteractionPanel->SetVisibility(ESlateVisibility::Hidden);
	NoticeMessageText->SetRenderOpacity(0.f);

	FillCircleMaterial = FillCircleImage->GetDynamicMaterial();

	LeftTimeFormatOptions.AlwaysSign = false;
	LeftTimeFormatOptions.UseGrouping = false;
	LeftTimeFormatOptions.RoundingMode = ToZero;
	LeftTimeFormatOptions.MinimumIntegralDigits = 1;
	LeftTimeFormatOptions.MaximumIntegralDigits = 2;
	LeftTimeFormatOptions.MinimumFractionalDigits = 1;
	LeftTimeFormatOptions.MaximumFractionalDigits = 1;
	
	GameInstance = Cast<UBattleRoyalGameInstance>(GetGameInstance());
	GameState = Cast<ABattleRoyalGameState>(UGameplayStatics::GetGameState(this));
	
	WeaponSlotImages[0] = PrimarySlotImage;
	WeaponSlotImages[1] = SecondarySlotImage;
	WeaponSlotImages[2] = TertiarySlotImage;

	RefreshIcon();
	RefreshAlpha();
}

void UNoticeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GameState)
		AliveNumText->SetText(FText::AsNumber(GameState->AliveCount));
	
	if (HealthKitPanel->GetVisibility() == ESlateVisibility::Visible)
	{
		LeftTime -= InDeltaTime;
		LeftTimeText->SetText(FText::AsNumber(LeftTime, &LeftTimeFormatOptions));
		FillCircleMaterial->SetScalarParameterValue(TEXT("Percent"), 1.0f - LeftTime / TotalTime);
		
		if (LeftTime <= 0.f)
			HideHealthKitUI();
	}

	ABattleRoyalCharacter* CharacterOwner = Cast<ABattleRoyalCharacter>(GetOwningPlayerPawn());
	if (IsValid(CharacterOwner) == false)
		return;
	
	const FItemObject& UsingWeapon = CharacterOwner->GetInventoryComponent()->GetUsingWeapon();
	if (UsingWeapon.ItemID == 0)
	{
		AmmoText->SetText(FText::FromString(TEXT("- / -")));
	}
	else
	{
		if (GameInstance == nullptr)
			return;
		
		const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(UsingWeapon.ItemID);
		if (ItemInfo == nullptr)
			return;
		
		FString AmmoString = FString::Printf(TEXT("%d / %d"), UsingWeapon.MagazineAmmoAmount, ItemInfo->MaxMagazineAmmoAmount);
		AmmoText->SetText(FText::FromString(AmmoString));
	}

	if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController()))
		KillNumText->SetText(FText::AsNumber(ControllerOwner->GetKillCount()));
}

void UNoticeWidget::ShowHealthKitUI(float Time, const FText& SubText, const FText& KeyText)
{
	TotalTime = Time;
	LeftTime = TotalTime;
	SubMessageText->SetText(SubText);
	KeyMessageText->SetText(KeyText);

	HealthKitPanel->SetVisibility(ESlateVisibility::Visible);
}

void UNoticeWidget::HideHealthKitUI()
{
	HealthKitPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UNoticeWidget::ShowNoticeText(const FString& NoticeString, bool bIsPersistant)
{
	if (bIsPersistant)
	{
		StopAnimation(NoticeMessageAnim);
		NoticeMessageText->SetText(FText::FromString(NoticeString));
		NoticeMessageText->SetRenderOpacity(1.f);
	}
	else
	{
		NoticeMessageText->SetText(FText::FromString(NoticeString));
		PlayAnimation(NoticeMessageAnim);
	}
}

void UNoticeWidget::ShowInteractionText(const FString& InteractionString)
{
	InteractionText->SetText(FText::FromString(InteractionString));
	InteractionPanel->SetVisibility(ESlateVisibility::Visible);
}

void UNoticeWidget::HideInteractionText()
{
	InteractionText->SetText(FText::GetEmpty());
	InteractionPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UNoticeWidget::RefreshIcon()
{
	ABattleRoyalCharacter* CharacterOwner = Cast<ABattleRoyalCharacter>(GetOwningPlayerPawn());
	if (IsValid(CharacterOwner) == false)
		return;
	
	TArray<FItemObject>& WeaponSlots = CharacterOwner->GetInventoryComponent()->GetWeaponSlots();
	for (int i = 0; i < WeaponSlotCount; i++)
	{
		const FItemObject& WeaponItem = WeaponSlots[i];
		if (WeaponItem.ItemID == 0)
		{
			WeaponSlotImages[i]->SetBrushFromTexture(UnarmedIcon);
		}
		else
		{
			if (GameInstance == nullptr)
				return;
			
			const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(WeaponItem.ItemID);
			if (ItemInfo == false)
				return;
			
			switch (ItemInfo->WeaponType)
			{
			case EWeaponType::Pistol:	WeaponSlotImages[i]->SetBrushFromTexture(PistolIcon);	break;
			case EWeaponType::Rifle:	WeaponSlotImages[i]->SetBrushFromTexture(RifleIcon);	break;
			case EWeaponType::Shotgun:	WeaponSlotImages[i]->SetBrushFromTexture(ShotgunIcon);	break;
			}
		}
	}
}

void UNoticeWidget::RefreshAlpha()
{
	for (UImage* SlotImage : WeaponSlotImages)
		SlotImage->SetRenderOpacity(0.3f);

	ABattleRoyalCharacter* CharacterOwner = Cast<ABattleRoyalCharacter>(GetOwningPlayerPawn());
	if (IsValid(CharacterOwner))
	{
		EWeaponSlotType UsingWeaponSlot = CharacterOwner->GetInventoryComponent()->GetUsingWeaponSlot();
		WeaponSlotImages[UsingWeaponSlot]->SetRenderOpacity(1.f);
	}
}
