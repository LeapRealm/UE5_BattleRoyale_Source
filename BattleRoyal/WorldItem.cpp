#include "WorldItem.h"

#include "BattleRoyalGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AWorldItem::AWorldItem()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionProfileName("IgnoreOnlyPawn");
}

void AWorldItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWorldItem, ItemObject);
}

void AWorldItem::Init(const FItemObject& NewItemObject)
{
	if (GetLocalRole() != ROLE_Authority)
		return;
	
	ItemObject = NewItemObject;
	RefreshMesh();
}

void AWorldItem::OnRep_ItemObject()
{
	RefreshMesh();
}

void AWorldItem::RefreshMesh()
{
	UBattleRoyalGameInstance* GameInstance = Cast<UBattleRoyalGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance == nullptr)
		return;
	
	const FItemBaseInfo* ItemBaseInfo = GameInstance->GetItemBaseInfo(ItemObject.ItemID);
	if (ItemBaseInfo == nullptr)
		return;
	
	MeshComponent->SetStaticMesh(ItemBaseInfo->StaticMesh);
}

void AWorldItem::Multicast_SetEnableCollision_Implementation(bool bIsEnable)
{
	SetActorEnableCollision(bIsEnable);
}
