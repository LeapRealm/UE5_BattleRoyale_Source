#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class BATTLEROYAL_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	
public:
	void RefreshUI(const FText& Nickname, bool bIsWinner, int32 KillNum, int32 RankNum, int32 TotalPlayerNum);

private:
	UFUNCTION()
	void GoToLobby();
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> NicknameText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ResultText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> KillNumText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> RankNumText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TotalPlayerNumText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> GoToLobbyButton;
};
