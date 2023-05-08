#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "NoticeWidget.generated.h"

class UImage;
class UTextBlock;
class UCanvasPanel;
class ABattleRoyalGameState;

UCLASS()
class BATTLEROYAL_API UNoticeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UNoticeWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
public:
	void ShowHealthKitUI(float Time, const FText& SubText, const FText& KeyText);
	void HideHealthKitUI();

	void ShowNoticeText(const FString& NoticeString, bool bIsPersistant = false);

	void ShowInteractionText(const FString& InteractionString);
	void HideInteractionText();
	
	void RefreshIcon();
	void RefreshAlpha();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBattleRoyalGameInstance> GameInstance;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ABattleRoyalGameState> GameState;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMaterialInstanceDynamic> FillCircleMaterial;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTexture2D> UnarmedIcon;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTexture2D> PistolIcon;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTexture2D> RifleIcon;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTexture2D> ShotgunIcon;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> HealthKitPanel;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> FillCircleImage;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> LeftTimeText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> SubMessageText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> KeyMessageText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> NoticeMessageText;
	
	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> NoticeMessageAnim;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PrimarySlotImage;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> SecondarySlotImage;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> TertiarySlotImage;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UImage> WeaponSlotImages[WeaponSlotCount];

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> AmmoText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> InteractionPanel;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> InteractionText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> AliveNumText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> KillNumText;

private:
	float TotalTime = 0.f;
	float LeftTime = 0.f;
	
	FNumberFormattingOptions LeftTimeFormatOptions;
};
