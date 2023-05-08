#pragma once

#include "CoreMinimal.h"
#include "WorldItem.h"
#include "Animation/AnimInstance.h"
#include "BattleRoyalAnimInstance.generated.h"

UCLASS()
class BATTLEROYAL_API UBattleRoyalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pawn", meta=(AllowPrivateAccess=true))
	float CurrLowerRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pawn", meta=(AllowPrivateAccess=true))
	float TargetLowerRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pawn", meta=(AllowPrivateAccess=true))
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pawn", meta=(AllowPrivateAccess=true))
	float AimPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pawn", meta=(AllowPrivateAccess=true))
	uint8 bIsFalling:1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pawn", meta=(AllowPrivateAccess=true))
	uint8 bIsCrouching:1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pawn", meta=(AllowPrivateAccess=true))
	EWeaponType UsingWeaponType;
};
