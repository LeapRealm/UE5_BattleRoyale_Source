#include "StatComponent.h"

#include "BattleRoyalCharacter.h"
#include "BattleRoyalGameState.h"
#include "BattleRoyalPlayerController.h"
#include "HUDWidget.h"
#include "HealthBarWidget.h"
#include "NoticeWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Death_Ironsights_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Characters/Animations/Actions/Death_Ironsights_Montage.Death_Ironsights_Montage'"));
	if (Death_Ironsights_Montage.Succeeded())
		DeathMontage = Death_Ironsights_Montage.Object;
}

void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterOwner = Cast<ABattleRoyalCharacter>(GetOwner());
	check(CharacterOwner);
	
	GameState = Cast<ABattleRoyalGameState>(UGameplayStatics::GetGameState(this));
	check(GameState);
	
	CurrHP = MaxHP;
}

void UStatComponent::TakeDamage(int32 DamageAmount)
{
	if (GetOwner()->HasAuthority() == false)
		return;

	if (IsValid(GameState) == false || GameState->bCanDamaged == false)
		return;
	
	if (CurrHP <= 0 || DamageAmount < 0)
		return;

	CurrHP = FMath::Max(CurrHP - DamageAmount, 0);
	if (CurrHP == 0)
	{
		CancelHeal();
		Client_Dead();
		Multicast_Dead();
		
		if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController()))
			ControllerOwner->OnDead();
	}

	Client_SetCurrHP(CurrHP, ECurrHPChangeType::TakeDamage);
}

void UStatComponent::InstantHeal(int32 HealAmount)
{
	if (GetOwner()->HasAuthority() == false)
		return;
	
	if (CurrHP >= MaxHP || HealAmount < 0)
		return;

	CurrHP = FMath::Min(CurrHP + HealAmount, MaxHP);

	Client_SetCurrHP(CurrHP, ECurrHPChangeType::InstanceHeal);
}

void UStatComponent::WaitForHeal(int32 HealAmount, float WaitTime)
{
	if (GetOwner()->HasAuthority() == false)
		return;
	
	if (WaitForHealHandle.IsValid())
		return;

	if (CurrHP >= MaxHP || HealAmount < 0 || WaitTime < 0)
		return;

	ApplyHealAmount = HealAmount;
	GetWorld()->GetTimerManager().SetTimer(WaitForHealHandle, this, &UStatComponent::ApplyHeal, WaitTime, false);
	Client_WaitForHeal((float)(CurrHP + HealAmount) / MaxHP);
}

void UStatComponent::ApplyHeal()
{
	if (GetOwner()->HasAuthority() == false)
		return;
	
	CurrHP = FMath::Min(CurrHP + ApplyHealAmount, MaxHP);
	
	Client_ApplyHeal(CurrHP);
	Multicast_CancelHeal();

	WaitForHealHandle.Invalidate();
	ApplyHealAmount = 0;
}

void UStatComponent::CancelHeal()
{
	if (GetOwner()->HasAuthority() == false)
		return;
	
	if (WaitForHealHandle.IsValid() == false)
		return;
	
	GetWorld()->GetTimerManager().ClearTimer(WaitForHealHandle);
	WaitForHealHandle.Invalidate();
	ApplyHealAmount = 0;
	
	Client_CancelHeal();
	Multicast_CancelHeal();
}

void UStatComponent::Multicast_CancelHeal_Implementation()
{
	if (IsValid(CharacterOwner) == false)
		return;
	
	CharacterOwner->SetIsHealCasting(false);
	CharacterOwner->GetCharacterMovement()->MaxWalkSpeed = CharacterOwner->DefaultWalkSpeed;
	CharacterOwner->StopAnimMontage(CharacterOwner->GetDrinkingMontage());
}

void UStatComponent::Server_CancelHeal_Implementation()
{
	CancelHeal();
}

void UStatComponent::Client_Dead_Implementation()
{
	if (IsValid(CharacterOwner) == false)
		return;
	
	ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController());
	if (ControllerOwner == nullptr)
		return;
	
	CharacterOwner->DisableInput(ControllerOwner);
}

void UStatComponent::Multicast_Dead_Implementation()
{
	if (IsValid(CharacterOwner) == false)
		return;
	
	CharacterOwner->SetIsDead(true);
	CharacterOwner->PlayAnimMontage(DeathMontage);
	CharacterOwner->GetCharacterMovement()->StopMovementImmediately();
	CharacterOwner->GetCharacterMovement()->SetMovementMode(MOVE_None);
	CharacterOwner->GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
	CharacterOwner->GetMesh()->SetCollisionProfileName("NoCollision");
}

void UStatComponent::Client_SetCurrHP_Implementation(int32 NewCurrHP, ECurrHPChangeType ChangeType)
{
	CurrHP = NewCurrHP;
	
	if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController()))
	{
		UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
		if (IsValid(HUDWidget) && IsValid(HUDWidget->HealthBarWidget))
			HUDWidget->HealthBarWidget->OnCurrHPChanged((float)CurrHP / MaxHP, ChangeType);
	}
}

void UStatComponent::Client_WaitForHeal_Implementation(float Ratio)
{
	if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController()))
	{
		UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
		if (IsValid(HUDWidget) && IsValid(HUDWidget->HealthBarWidget))
			HUDWidget->HealthBarWidget->WaitForHeal(Ratio);
	}
}

void UStatComponent::Client_ApplyHeal_Implementation(int32 NewCurrHP)
{
	ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController());
	if (ControllerOwner == nullptr)
		return;
	
	CurrHP = NewCurrHP;

	UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
	if (IsValid(HUDWidget))
	{
		HUDWidget->SetReticleVisible();

		if (IsValid(HUDWidget->HealthBarWidget))
			HUDWidget->HealthBarWidget->ApplyHeal();
	}
}

void UStatComponent::Client_CancelHeal_Implementation()
{
	ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController());
	if (ControllerOwner == nullptr)
		return;

	UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
	if (IsValid(HUDWidget))
	{
		HUDWidget->SetReticleVisible();

		if (IsValid(HUDWidget->NoticeWidget))
			HUDWidget->NoticeWidget->HideHealthKitUI();

		if (IsValid(HUDWidget->HealthBarWidget))
			HUDWidget->HealthBarWidget->CancelHeal();
	}
}
