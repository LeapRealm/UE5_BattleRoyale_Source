#include "BattleRoyalGameMode.h"

#include "BattleRoyalGameState.h"
#include "BattleRoyalCharacter.h"
#include "BattleRoyalPlayerController.h"
#include "Aircraft.h"
#include "ElectricField.h"
#include "HUDWidget.h"

#include "Kismet/GameplayStatics.h"

ABattleRoyalGameMode::ABattleRoyalGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	
	DefaultPawnClass = ABattleRoyalCharacter::StaticClass();
	PlayerControllerClass = ABattleRoyalPlayerController::StaticClass();
	GameStateClass = ABattleRoyalGameState::StaticClass();
	
	RemainWaitTime = DefaultWaitTime;
	RemainCeremonyTime = DefaultCeremonyTime;
}

void ABattleRoyalGameMode::BeginPlay()
{
	Super::BeginPlay();

	Aircraft = Cast<AAircraft>(UGameplayStatics::GetActorOfClass(GetWorld(), AAircraft::StaticClass()));
	ElectricField = GetWorld()->SpawnActor<AElectricField>(AElectricField::StaticClass(),FVector(0.f, 0.f, -10000.f), FRotator::ZeroRotator);

	GetWorldTimerManager().SetTimer(MainTimerHandle, this, &ABattleRoyalGameMode::MainTimer, 1.f, true);

	// TODO: For Test
	// if (UBattleRoyalGameInstance* GameInstance = Cast<UBattleRoyalGameInstance>(GetGameInstance()))
	// {
	// 	for (int i = 0; i < 160; i++)
	// 	{
	// 		AWorldItem* Item = GetWorld()->SpawnActor<AWorldItem>(AWorldItem::StaticClass(),
	// 			FVector(13350.f, -25294.f, 0.f) + FVector(FMath::RandPointInCircle(1000.f), 0.f), FRotator::ZeroRotator);
	// 		uint32 ItemID = FMath::RandRange(1, GameInstance->GetItemBaseInfoRowCount());
	//
	// 		if (const FItemBaseInfo* ItemBaseInfo = GameInstance->GetItemBaseInfo(ItemID))
	// 		{
	// 			int32 ItemAmount = (ItemBaseInfo->ItemType == EItemType::Consumable) ? FMath::RandRange(10, 50) : 1;
	// 			Item->Init({ItemID, ItemAmount});
	// 		}
	// 	}
	// }
}

void ABattleRoyalGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	if (GameplayState != EGameplayState::Wait)
	{
		if (ABattleRoyalPlayerController* PlayerController = Cast<ABattleRoyalPlayerController>(NewPlayer))
			PlayerController->Client_GoToLobby();
		else
			NewPlayer->SetLifeSpan(0.1f);
	}
	else
	{
		if (ABattleRoyalPlayerController* PlayerController = Cast<ABattleRoyalPlayerController>(NewPlayer))
		{
			FString String = FString::Printf(TEXT("플레이어%d"), PlayerController->GetUniqueID());
			PlayerController->SetNickname(FText::FromString(String));
		
			AlivePCs.Add(PlayerController);
		}
	}
}

void ABattleRoyalGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (ABattleRoyalPlayerController* PlayerController = Cast<ABattleRoyalPlayerController>(Exiting))
	{
		AlivePCs.Remove(PlayerController);
		DeadPCs.Remove(PlayerController);
	}
}

void ABattleRoyalGameMode::MainTimer()
{
	ABattleRoyalGameState* BattleRoyalGameState = GetGameState<ABattleRoyalGameState>();
	if (BattleRoyalGameState == nullptr)
		return;

	BattleRoyalGameState->AliveCount = AlivePCs.Num();
	
	if (GameplayState == EGameplayState::Wait)
	{
		if (bIsStarted)
		//if (AlivePCs.Num() >= TargetPlayerCount)
		{
			RemainWaitTime--;
			if (RemainWaitTime <= 0)
			{
				TotalPlayerCount = AlivePCs.Num();
				
				for (ABattleRoyalPlayerController* PlayerController : AlivePCs)
				{
					if (IsValid(PlayerController))
						PlayerController->RideInAircraft();
				}

				if (IsValid(Aircraft))
					Aircraft->SetStarted();

				BattleRoyalGameState->bCanDamaged = true;
				NoticeToAll(TEXT("전투를 시작합니다."));
				GameplayState = EGameplayState::Battle;
			}
			else
			{
				NoticeToAll(FString::Printf(TEXT("전투시작까지 %d초 남았습니다"), RemainWaitTime), true);
			}
		}
		else
		{
			RemainWaitTime = DefaultWaitTime;
			NoticeToAll(TEXT("다른 플레이어를 기다리는 중입니다"), true);
		}
	}
	else if (GameplayState == EGameplayState::Battle)
	{
		BattleRoyalGameState->UpdateElectricField();
		
		if (AlivePCs.Num() <= 1)
		{
			if (AlivePCs.Num() > 0 && IsValid(AlivePCs[0]))
				AlivePCs[0]->Client_ShowResultUI(true, 1, TotalPlayerCount);
			GameplayState = EGameplayState::Ceremony;
		}
		else
		{
			if (BattleRoyalGameState->bVisibleCurrCircle)
			{
				for (ABattleRoyalPlayerController* Controller : AlivePCs)
				{
					if (IsValid(Controller))
					{
						ABattleRoyalCharacter* Character = Cast<ABattleRoyalCharacter>(Controller->GetCharacter());
						if (IsValid(Character))
						{
							float Length = (FVector2D(Character->GetActorLocation()) - FVector2D(BattleRoyalGameState->CurrCirclePos)).Length();
							if (Length >= BattleRoyalGameState->CurrCircleRadius)
								UGameplayStatics::ApplyDamage(Character, ElectricField->Damage, nullptr,ElectricField, nullptr);
						}
					}
				}
			}
		}
	}
	else if (GameplayState == EGameplayState::Ceremony)
	{
		RemainCeremonyTime--;
		if (RemainCeremonyTime <= 0)
		{
			GetWorldTimerManager().ClearTimer(MainTimerHandle);
			MainTimerHandle.Invalidate();

			for (ABattleRoyalPlayerController* PlayerController : AlivePCs)
			{
				if (IsValid(PlayerController))
					PlayerController->Client_GoToLobby();
			}

			for (ABattleRoyalPlayerController* PlayerController : DeadPCs)
			{
				if (IsValid(PlayerController))
					PlayerController->Client_GoToLobby();
			}

			FName CurrentLevelName = FName(*UGameplayStatics::GetCurrentLevelName(GetWorld()));
			UGameplayStatics::OpenLevel(GetWorld(), CurrentLevelName, true, TEXT("listen"));
		}
	}
}

void ABattleRoyalGameMode::NoticeToAll(const FString& NoticeString, bool bPersistent)
{
	for (ABattleRoyalPlayerController* PlayerController : AlivePCs)
	{
		if (IsValid(PlayerController))
			PlayerController->Client_ShowNoticeText(NoticeString, bPersistent);
	}

	for (ABattleRoyalPlayerController* PlayerController : DeadPCs)
	{
		if (IsValid(PlayerController))
			PlayerController->Client_ShowNoticeText(NoticeString, bPersistent);
	}
}

void ABattleRoyalGameMode::OnDead(ABattleRoyalPlayerController* DeadController)
{
	if (IsValid(DeadController) == false)
		return;
	
	DeadController->Client_ShowResultUI(false, AlivePCs.Num(), TotalPlayerCount);
	
	AlivePCs.Remove(DeadController);
	DeadPCs.Add(DeadController);
}

void ABattleRoyalGameMode::AllDive()
{
	for (ABattleRoyalPlayerController* Controller : AlivePCs)
	{
		if (IsValid(Controller) && Controller->GetIsDived() == false)
			Controller->Server_RequestDive();
	}
}
