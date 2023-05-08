#include "BattleRoyalGameInstance.h"

#include "WorldItem.h"

#include "Engine/DataTable.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"

UBattleRoyalGameInstance::UBattleRoyalGameInstance()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> ItemBaseInfo(TEXT("/Script/Engine.DataTable'/Game/BattleRoyal/Data/ItemBaseInfo.ItemBaseInfo'"));
	if (ItemBaseInfo.Succeeded())
	{
		ItemBaseInfoTable = ItemBaseInfo.Object;
		ItemBaseInfoTableRowNum = ItemBaseInfoTable->GetRowNames().Num();
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> ElectricFieldPhaseInfo(TEXT("//Script/Engine.DataTable'/Game/BattleRoyal/Data/ElectricFieldPhaseInfo.ElectricFieldPhaseInfo'"));
	if (ElectricFieldPhaseInfo.Succeeded())
	{
		ElectricFieldPhaseTable = ElectricFieldPhaseInfo.Object;
		ElectricFieldPhaseTableRowNum = ElectricFieldPhaseTable->GetRowNames().Num();
	}
}

void UBattleRoyalGameInstance::Init()
{
	Super::Init();
	
	if (UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
		GameUserSettings->RunHardwareBenchmark();
		GameUserSettings->ApplyHardwareBenchmarkResults();
	}
}

const FItemBaseInfo* UBattleRoyalGameInstance::GetItemBaseInfo(uint32 ItemID)
{
	return (ItemID == 0) ? nullptr : ItemBaseInfoTable->FindRow<FItemBaseInfo>(*FString::FromInt(ItemID), TEXT(""));
}

const FElectricFieldPhase* UBattleRoyalGameInstance::GetElectricFieldPhase(int32 Phase)
{
	if (Phase < 1 || Phase > ElectricFieldPhaseTableRowNum)
		return nullptr;
	
	return ElectricFieldPhaseTable->FindRow<FElectricFieldPhase>(*FString::FromInt(Phase), TEXT(""));
}
