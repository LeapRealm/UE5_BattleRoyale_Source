#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldItem.h"
#include "InventoryComponent.generated.h"

class ABattleRoyalCharacter;
class ABattleRoyalPlayerController;
class UBattleRoyalGameInstance;

UENUM()
enum EWeaponSlotType
{
	Primary = 0,
	Secondary,
	Tertiary,

	WeaponSlotCount
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYAL_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
	
	virtual void BeginPlay() override;

public:
	// Interaction
	void Interaction();
	bool CanInteraction();
	
	UFUNCTION(Server, Reliable)
	void Server_InteractionDoor();
	
	UFUNCTION(Server, Reliable)
	void Server_InteractionItem();

	UFUNCTION(Server, Reliable)
	void Server_SearchNearItems();

	UFUNCTION(Server, Reliable)
	void Server_UnlockWorldItems();

	// Health Kit
	UFUNCTION(Server, Reliable)
	void Server_UseHealthKit();

	UFUNCTION(Client, Reliable)
	void Client_UseHealthKit(int32 SlotIndex);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UseHealthKit();

	// Consumable
	void AddConsumable(const FItemObject& NewItemObject);
	
	UFUNCTION(Client, Reliable)
	void Client_AddConsumable(const FItemObject& NewItemObject);

	void SubConsumable(uint32 SlotID, const FItemObject& NewItemObject, int32 Amount);

	UFUNCTION(Client, Reliable)
	void Client_SubConsumable(uint32 SlotID, const FItemObject& ItemObject, int32 Amount);
	
	// Weapon
	UFUNCTION(Server, Reliable)
	void Server_RequestSetWeapon(uint32 SlotID, const FItemObject& NewItemObject);

	UFUNCTION(Server, Reliable)
	void Server_RequestRemoveWeapon(int32 WeaponSlotIndex, const FItemObject& NewItemObject);
	
	void SetWeapon(int32 WeaponSlotIndex, const FItemObject& NewItemObject);
	
	UFUNCTION(Client, Reliable)
	void Client_SetWeapon(int32 WeaponSlotIndex, const FItemObject& NewItemObject);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RefreshWeaponMesh(uint32 ItemID);

	// Armor
	UFUNCTION(Server, Reliable)
	void Server_RequestSetArmor(uint32 SlotID, const FItemObject& NewItemObject);

	UFUNCTION(Server, Reliable)
	void Server_RequestRemoveArmor(EArmorType ArmorType, const FItemObject& NewItemObject);
	
	void SetArmor(EArmorType ArmorType, const FItemObject& NewItemObject);
	
	UFUNCTION(Client, Reliable)
	void Client_SetArmor(EArmorType ArmorType, const FItemObject& NewItemObject);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RefreshArmorMesh(EArmorType ArmorType, uint32 ItemID);
	
	// Weapon Slot
	void RequestSelectWeaponSlot(EWeaponSlotType NewSlotType);
	bool CanChangeWeaponSlot(EWeaponSlotType NewSlotType);

	UFUNCTION(Server, Reliable)
	void Server_SelectWeaponSlot(EWeaponSlotType NewSlotType);
	
	UFUNCTION()
	void OnRep_UsingWeaponSlot();

	// Move Item To Another Slot
	UFUNCTION(Server, Reliable)
	void Server_RequestMoveConsumable(EInventorySlotType From, uint32 SlotID, const FItemObject& NewItemObject);

	// Refresh UI
	UFUNCTION(Client, Reliable)
	void Client_RefreshSlotUI(const TArray<FItemObject>& NearItems);

	UFUNCTION(Client, Reliable)
	void Client_RefreshArmorUI();

	UFUNCTION(Client, Reliable)
	void Client_RefreshWeaponUI();

private:
	// Utils
	UFUNCTION()
	void LineTraceTimer();
	
	AWorldItem* FindSingleWorldItemFromLineTrace();
	FVector FindItemSpawnPos();
	
public:
	FORCEINLINE TArray<FItemObject>& GetInventoryItems() { return InventoryItems; }
	FORCEINLINE TArray<FItemObject>& GetWeaponSlots() { return WeaponSlots; }
	FORCEINLINE TArray<FItemObject>& GetArmorSlots() { return ArmorSlots; }
	FORCEINLINE FItemObject& GetUsingWeapon() { return WeaponSlots[UsingWeaponSlot]; }
	FORCEINLINE EWeaponType GetUsingWeaponType() const { return UsingWeaponType; }
	FORCEINLINE EWeaponSlotType GetUsingWeaponSlot() const { return UsingWeaponSlot; }
	
	FORCEINLINE void SetUsingWeaponType(EWeaponType NewWeaponType) { UsingWeaponType = NewWeaponType; }
	
	int32 GetTotalAttack();
	int32 GetTotalDefence();

private:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<AWorldItem>> NearWorldItems;

	UPROPERTY(VisibleAnywhere)
	TArray<FItemObject> InventoryItems;
	
	UPROPERTY(VisibleAnywhere)
	TArray<FItemObject> WeaponSlots;
	
	UPROPERTY(VisibleAnywhere)
	TArray<FItemObject> ArmorSlots;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_UsingWeaponSlot)
	TEnumAsByte<EWeaponSlotType> UsingWeaponSlot;

	UPROPERTY(VisibleAnywhere)
	EWeaponType UsingWeaponType;

	UPROPERTY(VisibleAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> SearchNearItemsObjectTypes;
	
	FTimerHandle LineTraceTimerHandle;

private:
	// Cache
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ABattleRoyalCharacter> CharacterOwner;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBattleRoyalGameInstance> GameInstance;

public:
	// TODO: Move to DataSheet
	float SearchNearItemsSphereRadius = 400.f;
};
