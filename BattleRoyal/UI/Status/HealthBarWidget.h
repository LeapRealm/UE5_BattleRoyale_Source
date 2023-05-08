#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UProgressBar;
class UStatComponent;
enum class ECurrHPChangeType : uint8;

UENUM()
enum class EHealthBarState : uint8
{
	Normal,
	TakeDamage,
	WaitForHeal,
};

UCLASS()
class BATTLEROYAL_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void OnCurrHPChanged(float NewRatio, ECurrHPChangeType ChangeType);
	void TakeDamage(float NewRatio);
	void InstantHeal(float NewRatio);
	
	void WaitForHeal(float NewRatio);
	void ApplyHeal();
	void CancelHeal();

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> BackgroundBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ForegroundBar;
	
private:
	EHealthBarState HealthBarState = EHealthBarState::Normal;
};
