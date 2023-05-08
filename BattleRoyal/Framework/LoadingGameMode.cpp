#include "LoadingGameMode.h"

#include "LoadingPlayerController.h"

ALoadingGameMode::ALoadingGameMode()
{
	PlayerControllerClass = ALoadingPlayerController::StaticClass();
}
