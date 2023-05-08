#include "TitleWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UTitleWidget::UTitleWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsFocusable = true;
}

void UTitleWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConnectButton->OnClicked.AddDynamic(this, &UTitleWidget::Connect);
	QuitButton->OnClicked.AddDynamic(this, &UTitleWidget::Quit);
}

void UTitleWidget::Connect()
{
	FString Options = FString::Printf(TEXT("NextMap=%s"), *IPAddressTextBox->GetText().ToString()); 
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("LoadingMap"), true, Options);
}

void UTitleWidget::Quit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}
