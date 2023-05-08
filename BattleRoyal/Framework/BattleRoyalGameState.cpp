#include "BattleRoyalGameState.h"

#include "BattleRoyalGameMode.h"
#include "BattleRoyalGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ABattleRoyalGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleRoyalGameState, AliveCount);
	DOREPLIFETIME(ABattleRoyalGameState, bCanDamaged);
	DOREPLIFETIME(ABattleRoyalGameState, CurrCirclePos);
	DOREPLIFETIME(ABattleRoyalGameState, CurrCircleRadius);
	DOREPLIFETIME(ABattleRoyalGameState, TargetCirclePos);
	DOREPLIFETIME(ABattleRoyalGameState, TargetCircleRadius);
	DOREPLIFETIME(ABattleRoyalGameState, bVisibleCurrCircle);
	DOREPLIFETIME(ABattleRoyalGameState, bVisibleTargetCircle);
}

void ABattleRoyalGameState::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		GameInstance = Cast<UBattleRoyalGameInstance>(UGameplayStatics::GetGameInstance(this));
		GameMode = Cast<ABattleRoyalGameMode>(UGameplayStatics::GetGameMode(this));
	
		SetElectricFieldPhase();
	}
}

void ABattleRoyalGameState::UpdateElectricField()
{
	if (HasAuthority() == false)
		return;
	
	if (CircleState == ECircleState::Wait)
	{
		WaitTime--;

		if (WaitTime < 0)
		{
			if (IsValid(GameMode))
				GameMode->NoticeToAll(TEXT("새로운 자기장 영역이 나타났습니다"));
			CircleState = ECircleState::ShowTargetCircle;
		}
	}
	else if (CircleState == ECircleState::ShowTargetCircle)
	{
		bVisibleTargetCircle = true;
		ShowTime--;

		if (ShowTime < 0)
		{
			if (IsValid(GameMode))
				GameMode->NoticeToAll(TEXT("자기장 영역이 줄어들기 시작합니다"));
			CircleState = ECircleState::ShrinkCircle;
		}
	}
	else if (CircleState == ECircleState::ShrinkCircle)
	{
		bVisibleCurrCircle = true;
		ShrinkTime--;

		if (ShrinkTime >= 0)
		{
			CurrCirclePos += DeltaPosPerSec;
			CurrCircleRadius += DeltaRadiusPerSec;
		}
		else
		{
			bVisibleTargetCircle = false;
			
			CurrPhase++;
			if (CurrPhase > GameInstance->GetElectricFieldPhaseRowCount())
			{
				CircleState = ECircleState::End;
				return;
			}

			SetElectricFieldPhase();
			CircleState = ECircleState::Wait;
		}
	}
}

void ABattleRoyalGameState::SetElectricFieldPhase()
{
	if (HasAuthority() == false)
		return;
	
	const FElectricFieldPhase* Info = GameInstance->GetElectricFieldPhase(CurrPhase);
	if (Info == nullptr)
		return;

	WaitTime = Info->WaitTime;
	ShowTime = Info->ShowTime;
	ShrinkTime = Info->ShrinkTime;
	TargetCircleRadius = Info->TargetCircleRadius;

	FVector RandDir = FMath::VRand();
	RandDir = FVector(RandDir.X, RandDir.Y, 0.f).GetSafeNormal();

	float MaxLength = CurrCircleRadius - TargetCircleRadius;
	float RandLength = FMath::RandRange(0.f, MaxLength);

	TargetCirclePos = RandDir * RandLength + CurrCirclePos;

	DeltaPosPerSec = (TargetCirclePos - CurrCirclePos) / ShrinkTime;
	DeltaRadiusPerSec = (TargetCircleRadius - CurrCircleRadius) / ShrinkTime;
}
