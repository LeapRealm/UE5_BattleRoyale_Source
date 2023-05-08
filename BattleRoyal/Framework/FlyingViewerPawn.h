#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "FlyingViewerPawn.generated.h"

class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BATTLEROYAL_API AFlyingViewerPawn : public APawn
{
	GENERATED_BODY()

public:
	AFlyingViewerPawn();
	
	virtual void BeginPlay() override;
	
public:
	void Look(const FInputActionValue& Value);
	void Interaction();
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> FlyingViewerMappingContext;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;
};
