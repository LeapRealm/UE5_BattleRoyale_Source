#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionWidget.generated.h"

class UButton;

UCLASS()
class BATTLEROYAL_API UOptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UOptionWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
private:
	UFUNCTION()
	void GoToLobby();
	
	UFUNCTION()
	void Quit();
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> GoToLobbyButton;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> QuitButton;
};
