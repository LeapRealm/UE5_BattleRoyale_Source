#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleRoyalGameMode.generated.h"

class AAircraft;
class AElectricField;
class ABattleRoyalPlayerController;

UENUM()
enum class EGameplayState : uint8
{
	Wait,
	Battle,
	Ceremony,
};

UCLASS(minimalapi)
class ABattleRoyalGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleRoyalGameMode();
	
	virtual void BeginPlay() override;
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

public:
	void NoticeToAll(const FString& NoticeString, bool bPersistent = false);
	void OnDead(ABattleRoyalPlayerController* DeadController);
	void AllDive();

private:
	UFUNCTION()
	void MainTimer();
	
private:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<ABattleRoyalPlayerController>> AlivePCs;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<ABattleRoyalPlayerController>> DeadPCs;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AAircraft> Aircraft;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AElectricField> ElectricField;
	
private:
	UPROPERTY(EditAnywhere)
	int32 TargetPlayerCount = 2;

	UPROPERTY(EditAnywhere)
	int32 DefaultWaitTime = 10;
	
	UPROPERTY(EditAnywhere)
	int32 DefaultCeremonyTime = 120;

private:
	UPROPERTY(VisibleAnywhere)
	int32 TotalPlayerCount;
	
	UPROPERTY(VisibleAnywhere)
	EGameplayState GameplayState = EGameplayState::Wait;

	UPROPERTY(VisibleAnywhere)
	int32 RemainWaitTime;

	UPROPERTY(VisibleAnywhere)
	int32 RemainCeremonyTime;
	
	FTimerHandle MainTimerHandle;

public:
	UPROPERTY(VisibleAnywhere)
	uint8 bIsStarted:1;
};
