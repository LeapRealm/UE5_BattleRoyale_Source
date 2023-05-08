#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LoadingPlayerController.generated.h"

class UUserWidget;
class ULoadingWidget;

UCLASS()
class BATTLEROYAL_API ALoadingPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALoadingPlayerController();
	
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> LoadingWidgetClass;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ULoadingWidget> LoadingWidget;
};
