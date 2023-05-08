#pragma once

#include "CoreMinimal.h"
#include "WorldItem.h"
#include "GameFramework/Actor.h"
#include "ItemSpawner.generated.h"

UCLASS()
class BATTLEROYAL_API AItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemSpawner();

protected:
	virtual void BeginPlay() override;

private:
	void SpawnRandomItem(EItemType ItemType, const FVector& Center, const FVector& BoxExtent);

private:
	// TODO: Move to Data Sheet
	int32 MinItemCnt = 8;
	int32 MaxItemCnt = 10;
};
