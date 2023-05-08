#include "LoadingPlayerController.h"

#include "LoadingWidget.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

ALoadingPlayerController::ALoadingPlayerController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_Loading(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BattleRoyal/UI/Widgets/WBP_Loading.WBP_Loading_C'"));
	if (WBP_Loading.Succeeded())
		LoadingWidgetClass = WBP_Loading.Class;
}

void ALoadingPlayerController::BeginPlay()
{
	Super::BeginPlay();

	LoadingWidget = CreateWidget<ULoadingWidget>(this, LoadingWidgetClass, TEXT("Title"));
	LoadingWidget->AddToViewport(0);
	
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(this, true);
	SetShowMouseCursor(false);

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	FString NextMap = UGameplayStatics::ParseOption(GameMode->OptionsString, TEXT("NextMap"));
	UGameplayStatics::OpenLevel(GetWorld(), FName(NextMap));
}
