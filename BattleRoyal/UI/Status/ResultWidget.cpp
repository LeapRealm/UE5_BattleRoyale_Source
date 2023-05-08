#include "ResultWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UResultWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	bIsFocusable = true;
	GoToLobbyButton->OnClicked.AddDynamic(this, &UResultWidget::GoToLobby);
}

void UResultWidget::RefreshUI(const FText& Nickname, bool bIsWinner, int32 KillNum, int32 RankNum, int32 TotalPlayerNum)
{
	NicknameText->SetText(Nickname);

	if (bIsWinner)
		ResultText->SetText(FText::FromString(TEXT("이겼닭! 오늘 저녁은 치킨이닭!")));
	else
		ResultText->SetText(FText::FromString(TEXT("그럴 수 있어. 이런 날도 있는 거지 뭐.")));

	KillNumText->SetText(FText::AsNumber(KillNum));

	FString String = FString::Printf(TEXT("#%d"), RankNum);
	RankNumText->SetText(FText::FromString(String));

	String = FString::Printf(TEXT("/%d"), TotalPlayerNum);
	TotalPlayerNumText->SetText(FText::FromString(String));
}

void UResultWidget::GoToLobby()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("TitleMap"));
}
