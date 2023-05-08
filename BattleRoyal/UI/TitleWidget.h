#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidget.generated.h"

class UButton;
class UEditableTextBox;

UCLASS()
class BATTLEROYAL_API UTitleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTitleWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeOnInitialized() override;
	
private:
	UFUNCTION()
	void Connect();
	
	UFUNCTION()
	void Quit();
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> IPAddressTextBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ConnectButton;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> QuitButton;
};
