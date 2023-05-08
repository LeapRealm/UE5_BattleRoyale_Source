#include "ItemSpawner.h"

#include "BattleRoyalGameInstance.h"
#include "ItemSpawnBoxComponent.h"
#include "WorldItem.h"

#include "Kismet/KismetMathLibrary.h"

AItemSpawner::AItemSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TArray<UItemSpawnBoxComponent*> ItemSpawnBoxes;
		GetComponents<UItemSpawnBoxComponent>(ItemSpawnBoxes);

		for (UItemSpawnBoxComponent* ItemSpawnBox : ItemSpawnBoxes)
		{
			SpawnRandomItem(EItemType::Weapon, ItemSpawnBox->GetComponentLocation(), ItemSpawnBox->GetScaledBoxExtent());

			for (int32 i = 0; i < 3; i++)
				SpawnRandomItem(EItemType::Armor, ItemSpawnBox->GetComponentLocation(), ItemSpawnBox->GetScaledBoxExtent());
			
			SpawnRandomItem(EItemType::Consumable, ItemSpawnBox->GetComponentLocation(), ItemSpawnBox->GetScaledBoxExtent());
		}
	}
}

void AItemSpawner::SpawnRandomItem(EItemType ItemType, const FVector& Center, const FVector& BoxExtent)
{
	if (HasAuthority() == false)
		return;

	UBattleRoyalGameInstance* GameInstance = Cast<UBattleRoyalGameInstance>(GetGameInstance());
	if (GameInstance == nullptr)
		return;
	
	uint32 ItemID = 0;
		
	if (ItemType == EItemType::Weapon)
		ItemID = FMath::RandRange(17, 19);
	else if (ItemType == EItemType::Armor)
		ItemID = FMath::RandRange(1, 12);
	else if (ItemType == EItemType::Consumable)
		ItemID = 13;
		
	FActorSpawnParameters Parameters;
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FVector Location = UKismetMathLibrary::RandomPointInBoundingBox(Center, BoxExtent / 2.f);
	AWorldItem* WorldItem = GetWorld()->SpawnActor<AWorldItem>(Location, FRotator::ZeroRotator, Parameters);
	
	if (GameInstance->GetItemBaseInfo(ItemID))
		WorldItem->Init(FItemObject(ItemID, 1));

	if (ItemType == EItemType::Weapon)
	{
		ItemID -= 3;

		Location = UKismetMathLibrary::RandomPointInBoundingBox(Center, BoxExtent / 2.f);
		WorldItem = GetWorld()->SpawnActor<AWorldItem>(Location, FRotator::ZeroRotator, Parameters);

		uint32 Amount = 0;
		const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(ItemID);
		if (ItemInfo)
		{
			switch (ItemInfo->ConsumableType)
			{
			case EConsumableType::AmmoPistol:	Amount = 24;	break;
			case EConsumableType::AmmoRifle:	Amount = 60;	break;
			case EConsumableType::AmmoShotgun:	Amount = 8;		break;
			}
		
			WorldItem->Init(FItemObject(ItemID, Amount));
		}
	}
}
