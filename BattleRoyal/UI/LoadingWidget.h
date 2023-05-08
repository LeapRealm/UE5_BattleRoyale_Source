#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingWidget.generated.h"

UCLASS()
class BATTLEROYAL_API ULoadingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	ULoadingWidget(const FObjectInitializer& ObjectInitializer);
};
