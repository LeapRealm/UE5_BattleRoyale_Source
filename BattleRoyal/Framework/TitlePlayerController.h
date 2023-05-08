#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TitlePlayerController.generated.h"

class UUserWidget;
class UTitleWidget;

UCLASS()
class BATTLEROYAL_API ATitlePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATitlePlayerController();
	
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> TitleWidgetClass;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTitleWidget> TitleWidget;
};
