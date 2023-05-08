#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BattleRoyalGameInstance.generated.h"

class UDataTable;
struct FElectricFieldPhase;
struct FItemBaseInfo;

UCLASS()
class BATTLEROYAL_API UBattleRoyalGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UBattleRoyalGameInstance();

	virtual void Init() override;

public:
	const FItemBaseInfo* GetItemBaseInfo(uint32 ItemID);
	FORCEINLINE int32 GetItemBaseInfoRowCount() const { return ItemBaseInfoTableRowNum; }

	const FElectricFieldPhase* GetElectricFieldPhase(int32 Phase);
	FORCEINLINE int32 GetElectricFieldPhaseRowCount() const { return ElectricFieldPhaseTableRowNum; }
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDataTable> ItemBaseInfoTable;

	UPROPERTY(VisibleAnywhere)
	int32 ItemBaseInfoTableRowNum;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDataTable> ElectricFieldPhaseTable;

	UPROPERTY(VisibleAnywhere)
	int32 ElectricFieldPhaseTableRowNum;
};
