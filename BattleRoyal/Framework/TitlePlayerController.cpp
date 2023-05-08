#include "TitlePlayerController.h"

#include "TitleWidget.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

ATitlePlayerController::ATitlePlayerController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_Title(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BattleRoyal/UI/Widgets/WBP_Title.WBP_Title_C'"));
	if (WBP_Title.Succeeded())
		TitleWidgetClass = WBP_Title.Class;
}

void ATitlePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	TitleWidget = CreateWidget<UTitleWidget>(this, TitleWidgetClass, TEXT("Title"));
	TitleWidget->AddToViewport(0);
	
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, TitleWidget, EMouseLockMode::DoNotLock, true);
	SetShowMouseCursor(true);
}
