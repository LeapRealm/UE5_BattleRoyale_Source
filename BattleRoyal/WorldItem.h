#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "WorldItem.generated.h"

struct FStreamableHandle;
class USphereComponent;
class UStaticMeshComponent;

UENUM()
enum class EItemType : uint8
{
	Weapon = 0,
	Armor,
	Consumable,

	None
};

UENUM()
enum class EWeaponType : uint8
{
	Pistol = 0,
	Rifle,
	Shotgun,

	None
};

UENUM()
enum class EArmorType : uint8
{
	Helmet = 0,
	Shirt,
	Vest,
	Backpack,
	Pants,
	Boots,
	
	None
};

enum
{
	ArmorSlotCount = EArmorType::None,
};

UENUM()
enum class EConsumableType : uint8
{
	HealthKit = 0,
	AmmoPistol,
	AmmoRifle,
	AmmoShotgun,
	
	None
};

USTRUCT()
struct FItemBaseInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	uint32 ItemID = 0;
	
	UPROPERTY(EditAnywhere)
	EItemType ItemType = EItemType::None;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType = EWeaponType::None;

	UPROPERTY(EditAnywhere)
	int32 Attack = 0;

	UPROPERTY(EditAnywhere)
	float FireRate = 0.f;

	UPROPERTY(EditAnywhere)
	int32 MaxMagazineAmmoAmount = 0;
	
	UPROPERTY(EditAnywhere)
	EArmorType ArmorType = EArmorType::None;

	UPROPERTY(EditAnywhere)
	int32 Defence = 0;
	
	UPROPERTY(EditAnywhere)
	EConsumableType ConsumableType = EConsumableType::None;

	UPROPERTY(EditAnywhere)
	float CastTime = 0.f;
	
	UPROPERTY(EditAnywhere)
	int32 RecoverAmount = 0;
	
	UPROPERTY(EditAnywhere)
	FText Name;
	
	UPROPERTY(EditAnywhere)
	FText Description;
	
	UPROPERTY(EditAnywhere)
	USkeletalMesh* SkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere)
	USkeletalMesh* SkeletalMeshAttachment = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMesh* StaticMesh = nullptr;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* Icon = nullptr;
};

USTRUCT()
struct FItemObject
{
	GENERATED_BODY()

	FItemObject() {}

	FItemObject(uint32 NewItemID, int32 NewAmount = 1, int32 NewMagazineAmmoAmount = 0)
		: ItemID(NewItemID), Amount(NewAmount), MagazineAmmoAmount(NewMagazineAmmoAmount) {}

	UPROPERTY(VisibleAnywhere)
	uint32 ItemID = 0;

	UPROPERTY(VisibleAnywhere)
	int32 Amount = 0;

	UPROPERTY(VisibleAnywhere)
	int32 MagazineAmmoAmount = 0;

	void Reset()
	{
		ItemID = 0;
		Amount = 0;
		MagazineAmmoAmount = 0;
	}

	bool operator==(const FItemObject& ItemObject) const
	{
		return	ItemObject.ItemID == ItemID &&
				ItemObject.Amount == Amount &&
				ItemObject.MagazineAmmoAmount == MagazineAmmoAmount;
	}

	bool operator!=(const FItemObject& ItemObject) const
	{
		return	!(ItemObject == *this);
	}
};

UCLASS()
class BATTLEROYAL_API AWorldItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldItem();
	
	void Init(const FItemObject& NewItemObject);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetEnableCollision(bool bIsEnable);
	
private:
	UFUNCTION()
	void OnRep_ItemObject();
	
	void RefreshMesh();

public:
	FORCEINLINE const FItemObject& GetItemObject() const { return ItemObject; }
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(ReplicatedUsing=OnRep_ItemObject)
	FItemObject ItemObject;
};
