#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryHoverWidget.generated.h"

class UTextBlock;
class UBattleRoyalGameInstance;

UCLASS()
class BATTLEROYAL_API UInventoryHoverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	
public:
	void RefreshUI(uint32 ItemID);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBattleRoyalGameInstance> GameInstance;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> NameText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemTypeText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> OptionalText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> DescriptionText;

private:
	FNumberFormattingOptions FireRateFormatOptions;
};
