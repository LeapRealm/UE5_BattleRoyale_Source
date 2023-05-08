#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CompassBarWidget.generated.h"

class UImage;

UCLASS()
class BATTLEROYAL_API UCompassBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    void ShowPinIcon(const FVector& WorldPos);
    void HidePinIcon();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMaterialInstanceDynamic> PinIconMaterial;
    
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMaterialInstanceDynamic> CompassMaterial;
	
private:
    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UImage> PinIconImage;
    
    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UImage> CompassImage;

private:
	UPROPERTY(VisibleAnywhere)
    uint8 bIsTargetSet:1;

	UPROPERTY(VisibleAnywhere)
    FVector TargetWorldPos;
};
