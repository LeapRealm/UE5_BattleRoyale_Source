#include "OptionWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UOptionWidget::UOptionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsFocusable = true;
}

void UOptionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GoToLobbyButton->OnClicked.AddDynamic(this, &UOptionWidget::GoToLobby);
	QuitButton->OnClicked.AddDynamic(this, &UOptionWidget::Quit);
}

FReply UOptionWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	
	if (InKeyEvent.IsRepeat() == false && InKeyEvent.GetKey() == EKeys::Escape && GetVisibility() == ESlateVisibility::Visible)
	{
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

void UOptionWidget::GoToLobby()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("TitleMap"));
}

void UOptionWidget::Quit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}
