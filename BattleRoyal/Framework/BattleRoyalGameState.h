#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/GameStateBase.h"
#include "BattleRoyalGameState.generated.h"

class ABattleRoyalGameMode;
class UBattleRoyalGameInstance;

UENUM()
enum class ECircleState : uint8
{
	Wait,
	ShowTargetCircle,
	ShrinkCircle,
	End,
};

USTRUCT()
struct FElectricFieldPhase : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 Phase = 0;

	UPROPERTY(EditAnywhere)
	float TargetCircleRadius = 0.f;

	UPROPERTY(EditAnywhere)
	int32 WaitTime = 0;

	UPROPERTY(EditAnywhere)
	int32 ShowTime = 0;

	UPROPERTY(EditAnywhere)
	int32 ShrinkTime = 0;
};

UCLASS()
class BATTLEROYAL_API ABattleRoyalGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
public:
	void UpdateElectricField();

private:
	void SetElectricFieldPhase();
	
public:
	UPROPERTY(Replicated)
	int32 AliveCount;

	UPROPERTY(Replicated)
	uint8 bCanDamaged:1;

	UPROPERTY(Replicated)
	FVector CurrCirclePos = FVector(201634.f, -201994.1f, 0.f);

	UPROPERTY(Replicated)
	float CurrCircleRadius = 241356.3f;

	UPROPERTY(Replicated)
	FVector TargetCirclePos;

	UPROPERTY(Replicated)
	float TargetCircleRadius;
	
	UPROPERTY(Replicated)
	uint8 bVisibleCurrCircle:1;

	UPROPERTY(Replicated)
	uint8 bVisibleTargetCircle:1;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBattleRoyalGameInstance> GameInstance;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ABattleRoyalGameMode> GameMode;
	
private:
	UPROPERTY(VisibleAnywhere)
	int32 CurrPhase = 1;

	UPROPERTY(VisibleAnywhere)
	ECircleState CircleState = ECircleState::Wait;

	UPROPERTY(VisibleAnywhere)
	int32 WaitTime;

	UPROPERTY(VisibleAnywhere)
	int32 ShowTime;
	
	UPROPERTY(VisibleAnywhere)
	int32 ShrinkTime;

	UPROPERTY(VisibleAnywhere)
	FVector DeltaPosPerSec = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere)
	float DeltaRadiusPerSec = 0.f;
};
