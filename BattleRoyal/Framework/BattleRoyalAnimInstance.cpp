#include "BattleRoyalAnimInstance.h"

#include "InventoryComponent.h"
#include "BattleRoyalCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

void UBattleRoyalAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ABattleRoyalCharacter* Character = Cast<ABattleRoyalCharacter>(TryGetPawnOwner());
	if (IsValid(Character) == false)
		return;

	bIsFalling = Character->GetCharacterMovement()->IsFalling();
	bIsCrouching = Character->GetCharacterMovement()->IsCrouching();
	UsingWeaponType = Character->GetInventoryComponent()->GetUsingWeaponType();
			
	FVector Velocity = Character->GetVelocity();
	Speed = Velocity.Size();
			
	if (Character->GetMoveForwardValue() == 0.f)
	{
		TargetLowerRotation = Character->GetMoveRightValue() * 60.f;
	}
	else
	{
		TargetLowerRotation = Character->GetMoveRightValue() * 30.f;

		if (Character->GetMoveForwardValue() < 0.f)
		{
			Speed *= -1;
			TargetLowerRotation *= -1;
		}
	}

	CurrLowerRotation = FMath::FInterpTo(CurrLowerRotation, TargetLowerRotation, DeltaSeconds, 10.f);
	AimPitch = (UsingWeaponType == EWeaponType::None || Character->GetIsHealCasting()) ? 0.f : Character->GetCurrAimPitch() / 2.f;
	AimPitch = FMath::Clamp(AimPitch, -45.f, 35.f);
}
