#include "InventoryComponent.h"

#include "Door.h"
#include "StatComponent.h"
#include "BattleRoyalCharacter.h"
#include "BattleRoyalGameInstance.h"
#include "BattleRoyalPlayerController.h"
#include "HUDWidget.h"
#include "NoticeWidget.h"
#include "InventoryWidget.h"
#include "InventorySlotWidget.h"
#include "WorldItem.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	UsingWeaponSlot = Primary;
	UsingWeaponType = EWeaponType::None;
	
	SearchNearItemsObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

	WeaponSlots.Init(FItemObject(), WeaponSlotCount);
	ArmorSlots.Init(FItemObject(), ArmorSlotCount);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterOwner = Cast<ABattleRoyalCharacter>(GetOwner());
	check(CharacterOwner);
	
	GameInstance = Cast<UBattleRoyalGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(GameInstance);
	
	if (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy && IsValid(CharacterOwner))
		GetWorld()->GetTimerManager().SetTimer(LineTraceTimerHandle, this, &UInventoryComponent::LineTraceTimer, 0.25f, true);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, UsingWeaponSlot);
}

void UInventoryComponent::Interaction()
{
	if (CanInteraction() == false)
		return;

	if (IsValid(CharacterOwner->GetOverlappedDoor()))
		Server_InteractionDoor();
	else if (IsValid(FindSingleWorldItemFromLineTrace()))
		Server_InteractionItem();
}

bool UInventoryComponent::CanInteraction()
{
	if (IsValid(CharacterOwner) == false)
		return false;

	return !(CharacterOwner->GetIsDead() || CharacterOwner->GetIsHealCasting() ||
			CharacterOwner->GetCharacterMovement()->IsSwimming());
}

void UInventoryComponent::Server_InteractionDoor_Implementation()
{
	if (CanInteraction() == false)
		return;

	if (IsValid(CharacterOwner))
	{
		ADoor* OverlappedDoor = CharacterOwner->GetOverlappedDoor();
		if (IsValid(OverlappedDoor))
			OverlappedDoor->RotateDoor(CharacterOwner->GetActorLocation());
	}
}

void UInventoryComponent::Server_InteractionItem_Implementation()
{
	if (CanInteraction() == false)
		return;
	
	AWorldItem* WorldItem = FindSingleWorldItemFromLineTrace();
	if (IsValid(WorldItem) == false)
		return;
	
	if (IsValid(GameInstance) == false)
		return;
	
	FItemObject WorldItemObject = WorldItem->GetItemObject();
	const FItemBaseInfo* WorldItemInfo = GameInstance->GetItemBaseInfo(WorldItemObject.ItemID);
	if (WorldItemInfo == nullptr)
		return;
	
	if (WorldItemInfo->ItemType == EItemType::Weapon)
	{
		bool bFound = false;
				
		for (int32 WeaponSlotIndex = 0; WeaponSlotIndex < WeaponSlotCount; WeaponSlotIndex++)
		{
			if (WeaponSlots[WeaponSlotIndex].ItemID == 0)
			{
				bFound = true;

				SetWeapon(WeaponSlotIndex, WorldItemObject);
				Client_SetWeapon(WeaponSlotIndex, WorldItemObject);
						
				if (WeaponSlotIndex == UsingWeaponSlot)
					Multicast_RefreshWeaponMesh(WorldItemObject.ItemID);

				WorldItem->Destroy();
				break;
			}
		}

		if (bFound == false)
		{
			FItemObject ItemObject = WeaponSlots[UsingWeaponSlot];

			SetWeapon(UsingWeaponSlot, WorldItemObject);
			Client_SetWeapon(UsingWeaponSlot, WorldItemObject);
			Multicast_RefreshWeaponMesh(WorldItemObject.ItemID);

			WorldItem->Init(ItemObject);
		}
	}
	else if (WorldItemInfo->ItemType == EItemType::Armor)
	{
		if (ArmorSlots[(uint8)WorldItemInfo->ArmorType].ItemID != 0)
		{
			FItemObject ItemObject = ArmorSlots[(uint8)WorldItemInfo->ArmorType];

			SetArmor(WorldItemInfo->ArmorType, WorldItemObject);
			Client_SetArmor(WorldItemInfo->ArmorType, WorldItemObject);
			Multicast_RefreshArmorMesh(WorldItemInfo->ArmorType, WorldItemObject.ItemID);

			WorldItem->Init(ItemObject);
		}
		else
		{
			SetArmor(WorldItemInfo->ArmorType, WorldItemObject);
			Client_SetArmor(WorldItemInfo->ArmorType, WorldItemObject);
			Multicast_RefreshArmorMesh(WorldItemInfo->ArmorType, WorldItemObject.ItemID);

			WorldItem->Destroy();
		}
	}
	else if (WorldItemInfo->ItemType == EItemType::Consumable)
	{
		AddConsumable(WorldItemObject);
		Client_AddConsumable(WorldItemObject);
		WorldItem->Destroy();
	}
}

void UInventoryComponent::Server_SearchNearItems_Implementation()
{
	TArray<AActor*> IgnoreActors;
	TArray<AActor*> OutActors;
	TArray<FItemObject> NearItems;
	
	if (IsValid(CharacterOwner) == false)
		return;
	
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), CharacterOwner->GetActorLocation(), SearchNearItemsSphereRadius,
		SearchNearItemsObjectTypes, AWorldItem::StaticClass(), IgnoreActors, OutActors);
	if (bHit)
	{
		for (int i = 0; i < OutActors.Num(); i++)
		{
			AWorldItem* WorldItem = Cast<AWorldItem>(OutActors[i]);
			if (IsValid(WorldItem))
			{
				NearWorldItems.Add(WorldItem);
				WorldItem->Multicast_SetEnableCollision(false);
				NearItems.Emplace(WorldItem->GetItemObject());
			}
		}
	}
	
	if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController()))
		ControllerOwner->Client_OpenInventory(NearItems);
}

void UInventoryComponent::Server_UnlockWorldItems_Implementation()
{
	for (int i = 0; i < NearWorldItems.Num(); i++)
	{
		if (IsValid(NearWorldItems[i]))
			NearWorldItems[i]->Multicast_SetEnableCollision(true);
	}
	NearWorldItems.Empty();
}

void UInventoryComponent::Server_UseHealthKit_Implementation()
{
	if (IsValid(CharacterOwner) == false || CharacterOwner->CanUseHealthKit() == false)
		return;

	if (IsValid(GameInstance) == false)
		return;
	
	bool bFound = false;
	
	for (int32 SlotIndex = 0; SlotIndex < InventoryItems.Num(); SlotIndex++)
	{
		const FItemBaseInfo* InvenItemInfo = GameInstance->GetItemBaseInfo(InventoryItems[SlotIndex].ItemID);
		if (InvenItemInfo && InvenItemInfo->ConsumableType == EConsumableType::HealthKit)
		{
			if (InventoryItems[SlotIndex].Amount <= 0)
				continue;

			bFound = true;
			InventoryItems[SlotIndex].Amount--;
			if (InventoryItems[SlotIndex].Amount <= 0)
				InventoryItems.RemoveAt(SlotIndex);
			
			CharacterOwner->GetStatComponent()->WaitForHeal(InvenItemInfo->RecoverAmount, InvenItemInfo->CastTime);
			
			Client_UseHealthKit(SlotIndex);
			Multicast_UseHealthKit();
			break;
		}
	}

	if (bFound == false)
	{
		if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController()))
			ControllerOwner->Client_ShowNoticeText(TEXT("인벤토리에 회복 아이템이 없습니다"));
	}
}

void UInventoryComponent::Client_UseHealthKit_Implementation(int32 SlotIndex)
{
	if (IsValid(GameInstance) == false)
		return;
	
	const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(InventoryItems[SlotIndex].ItemID);
	if (ItemInfo == nullptr)
		return;
	
	if (ItemInfo->ConsumableType == EConsumableType::HealthKit && InventoryItems[SlotIndex].Amount > 0)
	{
		InventoryItems[SlotIndex].Amount--;
		if (InventoryItems[SlotIndex].Amount <= 0)
			InventoryItems.RemoveAt(SlotIndex);

		if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController()))
		{
			UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
			if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
			{
				HUDWidget->InventoryWidget->RefreshInventorySlotUI(CharacterOwner->GetInventoryComponent()->InventoryItems);
				HUDWidget->NoticeWidget->ShowHealthKitUI(ItemInfo->CastTime,
				FText::FromString(TEXT("상처 치료 중")), FText::FromString(TEXT("버리기")));
				HUDWidget->SetReticleInvisible();
			}
		}
	}
}

void UInventoryComponent::Multicast_UseHealthKit_Implementation()
{
	if (IsValid(CharacterOwner) == false)
		return;
	
	CharacterOwner->SetIsHealCasting(true);
	CharacterOwner->GetCharacterMovement()->MaxWalkSpeed = CharacterOwner->DefaultCrouchSpeed;
	CharacterOwner->PlayAnimMontage(CharacterOwner->GetDrinkingMontage());
}

void UInventoryComponent::AddConsumable(const FItemObject& NewItemObject)
{
	bool bFound = false;

	if (IsValid(GameInstance) == false)
		 return;
	
	const FItemBaseInfo* NewItemInfo = GameInstance->GetItemBaseInfo(NewItemObject.ItemID);
	if (NewItemInfo == nullptr)
		return;
	
	for (FItemObject& InventoryItem : InventoryItems)
	{
		const FItemBaseInfo* InvenItemInfo = GameInstance->GetItemBaseInfo(InventoryItem.ItemID);
		if (InvenItemInfo && NewItemInfo->ConsumableType == InvenItemInfo->ConsumableType)
		{
			bFound = true;
			InventoryItem.Amount += NewItemObject.Amount;
			break;
		}
	}

	if (bFound == false)
		InventoryItems.Emplace(NewItemObject);
}

void UInventoryComponent::Client_AddConsumable_Implementation(const FItemObject& NewItemObject)
{
	AddConsumable(NewItemObject);
}

void UInventoryComponent::SubConsumable(uint32 SlotID, const FItemObject& NewItemObject, int32 Amount)
{
	if (SlotID >= (uint32)InventoryItems.Num())
		return;

	if (InventoryItems[SlotID] != NewItemObject || NewItemObject.Amount < Amount)
		return;

	if (IsValid(GameInstance) == false)
		return;
	
	const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(InventoryItems[SlotID].ItemID);
	if (ItemInfo == nullptr || ItemInfo->ItemType != EItemType::Consumable)
		return;

	InventoryItems[SlotID].Amount -= Amount;
	if (InventoryItems[SlotID].Amount <= 0)
		InventoryItems.RemoveAt(SlotID);
}

void UInventoryComponent::Client_SubConsumable_Implementation(uint32 SlotID, const FItemObject& ItemObject, int32 Amount)
{
	SubConsumable(SlotID, ItemObject, Amount);
}

void UInventoryComponent::Server_RequestSetWeapon_Implementation(uint32 SlotID, const FItemObject& NewItemObject)
{
	if (SlotID >= (uint32)NearWorldItems.Num())
		return;
		
	AWorldItem* WorldItem = NearWorldItems[SlotID];
	if (IsValid(WorldItem) == false || WorldItem->GetItemObject() != NewItemObject)
		return;

	if (IsValid(GameInstance) == false)
		return;
	
	const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(WorldItem->GetItemObject().ItemID);
	if (ItemInfo == nullptr || ItemInfo->ItemType != EItemType::Weapon)
		return;

	bool bFound = false;
	
	for (int32 WeaponSlotIndex = 0; WeaponSlotIndex < WeaponSlotCount; WeaponSlotIndex++)
	{
		if (WeaponSlots[WeaponSlotIndex].ItemID == 0)
		{
			bFound = true;
	
			SetWeapon(WeaponSlotIndex, NewItemObject);
			Client_SetWeapon(WeaponSlotIndex, NewItemObject);
						
			if (WeaponSlotIndex == UsingWeaponSlot)
				Multicast_RefreshWeaponMesh(NewItemObject.ItemID);
	
			NearWorldItems.RemoveAt(SlotID);
			WorldItem->Destroy();
			break;
		}
	}
	
	if (bFound == false)
	{
		FItemObject ItemObject = WeaponSlots[UsingWeaponSlot];
	
		SetWeapon(UsingWeaponSlot, NewItemObject);
		Client_SetWeapon(UsingWeaponSlot, NewItemObject);
		Multicast_RefreshWeaponMesh(NewItemObject.ItemID);
	
		WorldItem->Init(ItemObject);
	}
	
	TArray<FItemObject> NearItems;
	for (int i = 0; i < NearWorldItems.Num(); i++)
	{
		AWorldItem* NearWorldItem = NearWorldItems[i];
		if (IsValid(NearWorldItem))
			NearItems.Emplace(NearWorldItem->GetItemObject());
	}
	Client_RefreshSlotUI(NearItems);
	
	Client_RefreshWeaponUI();
}

void UInventoryComponent::Server_RequestRemoveWeapon_Implementation(int32 WeaponSlotIndex, const FItemObject& NewItemObject)
{
	if (NewItemObject.ItemID == 0)
		return;

	if (WeaponSlotIndex < 0 || WeaponSlotIndex >= EWeaponSlotType::WeaponSlotCount)
		return;
	
	if (WeaponSlots[WeaponSlotIndex] != NewItemObject)
		return;

	AWorldItem* WorldItem = GetWorld()->SpawnActor<AWorldItem>(AWorldItem::StaticClass(), FindItemSpawnPos(), FRotator::ZeroRotator);
	if (IsValid(WorldItem) == false)
		return;
	
	WorldItem->Init(NewItemObject);
	WorldItem->Multicast_SetEnableCollision(false);
	NearWorldItems.Add(WorldItem);

	SetWeapon(WeaponSlotIndex, FItemObject());
	Client_SetWeapon(WeaponSlotIndex, FItemObject());

	if (WeaponSlotIndex == UsingWeaponSlot)
		Multicast_RefreshWeaponMesh(0);
	
	TArray<FItemObject> NearItems;
	for (int i = 0; i < NearWorldItems.Num(); i++)
	{
		AWorldItem* NearWorldItem = NearWorldItems[i];
		if (IsValid(NearWorldItem))
			NearItems.Emplace(NearWorldItem->GetItemObject());
	}
	Client_RefreshSlotUI(NearItems);

	Client_RefreshWeaponUI();
}

void UInventoryComponent::SetWeapon(int32 WeaponSlotIndex, const FItemObject& NewItemObject)
{
	WeaponSlots[WeaponSlotIndex] = NewItemObject;
}

void UInventoryComponent::Client_SetWeapon_Implementation(int32 WeaponSlotIndex, const FItemObject& NewItemObject)
{
	ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController());
	if (ControllerOwner == nullptr)
		return;

	SetWeapon(WeaponSlotIndex, NewItemObject);

	UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
	if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
		HUDWidget->NoticeWidget->RefreshIcon();
}

void UInventoryComponent::Multicast_RefreshWeaponMesh_Implementation(uint32 ItemID)
{
	if (IsValid(CharacterOwner) == false)
		return;
	
	if (ItemID == 0)
	{
		CharacterOwner->SetWeaponMesh(EWeaponType::None, nullptr);
	}
	else
	{
		if (IsValid(GameInstance))
		{
			const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(ItemID);
			if (ItemInfo)
				CharacterOwner->SetWeaponMesh(ItemInfo->WeaponType, ItemInfo->SkeletalMesh);
		}
	}
}

void UInventoryComponent::Server_RequestSetArmor_Implementation(uint32 SlotID, const FItemObject& NewItemObject)
{
	if (SlotID >= (uint32)NearWorldItems.Num())
		return;
		
	AWorldItem* WorldItem = NearWorldItems[SlotID];
	if (IsValid(WorldItem) == false || WorldItem->GetItemObject() != NewItemObject)
		return;

	if (IsValid(GameInstance) == false)
		return;
	
	const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(WorldItem->GetItemObject().ItemID);
	if (ItemInfo == nullptr || ItemInfo->ItemType != EItemType::Armor)
		return;
	
	if (ArmorSlots[(uint8)ItemInfo->ArmorType].ItemID != 0)
	{
		FItemObject ItemObject = ArmorSlots[(uint8)ItemInfo->ArmorType];

		SetArmor(ItemInfo->ArmorType, NewItemObject);
		Client_SetArmor(ItemInfo->ArmorType, NewItemObject);
		Multicast_RefreshArmorMesh(ItemInfo->ArmorType, NewItemObject.ItemID);

		WorldItem->Init(ItemObject);
	}
	else
	{
		SetArmor(ItemInfo->ArmorType, NewItemObject);
		Client_SetArmor(ItemInfo->ArmorType, NewItemObject);
		Multicast_RefreshArmorMesh(ItemInfo->ArmorType, NewItemObject.ItemID);
		
		NearWorldItems.RemoveAt(SlotID);
		WorldItem->Destroy();
	}

	TArray<FItemObject> NearItems;
	for (int i = 0; i < NearWorldItems.Num(); i++)
	{
		AWorldItem* NearWorldItem = NearWorldItems[i];
		if (IsValid(NearWorldItem))
			NearItems.Emplace(NearWorldItem->GetItemObject());
	}
	Client_RefreshSlotUI(NearItems);

	Client_RefreshArmorUI();
}

void UInventoryComponent::Server_RequestRemoveArmor_Implementation(EArmorType ArmorType, const FItemObject& NewItemObject)
{
	if (NewItemObject.ItemID == 0 || ArmorType == EArmorType::None)
		return;
	
	if (ArmorSlots[(uint8)ArmorType] != NewItemObject)
		return;

	AWorldItem* WorldItem = GetWorld()->SpawnActor<AWorldItem>(AWorldItem::StaticClass(), FindItemSpawnPos(), FRotator::ZeroRotator);
	if (IsValid(WorldItem) == false)
		return;
		
	WorldItem->Init(NewItemObject);
	WorldItem->Multicast_SetEnableCollision(false);
	NearWorldItems.Add(WorldItem);
	
	SetArmor(ArmorType, FItemObject());
	Client_SetArmor(ArmorType, FItemObject());
	Multicast_RefreshArmorMesh(ArmorType, 0);
	
	TArray<FItemObject> NearItems;
	for (int i = 0; i < NearWorldItems.Num(); i++)
	{
		AWorldItem* NearWorldItem = NearWorldItems[i];
		if (IsValid(NearWorldItem))
			NearItems.Emplace(NearWorldItem->GetItemObject());
	}
	Client_RefreshSlotUI(NearItems);

	Client_RefreshArmorUI();
}

void UInventoryComponent::SetArmor(EArmorType ArmorType, const FItemObject& NewItemObject)
{
	ArmorSlots[(uint8)ArmorType] = NewItemObject;
}

void UInventoryComponent::Client_SetArmor_Implementation(EArmorType ArmorType, const FItemObject& NewItemObject)
{
	SetArmor(ArmorType, NewItemObject);
}

void UInventoryComponent::Multicast_RefreshArmorMesh_Implementation(EArmorType ArmorType, uint32 ItemID)
{
	if (IsValid(CharacterOwner) == false)
		return;
	
	if (ItemID == 0)
	{
		CharacterOwner->SetArmorMesh(ArmorType, nullptr, nullptr);
	}
	else
	{
		if (IsValid(GameInstance))
		{
			const FItemBaseInfo* ItemBaseInfo = GameInstance->GetItemBaseInfo(ItemID);
			if (ItemBaseInfo)
				CharacterOwner->SetArmorMesh(ArmorType, ItemBaseInfo->SkeletalMesh, ItemBaseInfo->SkeletalMeshAttachment);
		}
	}
}

void UInventoryComponent::RequestSelectWeaponSlot(EWeaponSlotType NewSlotType)
{
	if (UsingWeaponSlot == NewSlotType || CanChangeWeaponSlot(NewSlotType) == false)
		return;

	Server_SelectWeaponSlot(NewSlotType);
}

bool UInventoryComponent::CanChangeWeaponSlot(EWeaponSlotType NewSlotType)
{
	if (IsValid(CharacterOwner) == false)
		return false;

	if (WeaponSlots[NewSlotType].ItemID == 0)
		return false;
	
	return !(CharacterOwner->GetIsADSing() || CharacterOwner->GetIsFiring() || CharacterOwner->GetIsReloading() || CharacterOwner->GetIsDead() ||
		CharacterOwner->GetIsHealCasting() || CharacterOwner->GetCharacterMovement()->IsSwimming() || CharacterOwner->GetCharacterMovement()->IsFalling());
}

void UInventoryComponent::Server_SelectWeaponSlot_Implementation(EWeaponSlotType NewSlotType)
{
	if (UsingWeaponSlot == NewSlotType || CanChangeWeaponSlot(NewSlotType) == false)
		return;
	
	UsingWeaponSlot = NewSlotType;
	Multicast_RefreshWeaponMesh(WeaponSlots[UsingWeaponSlot].ItemID);
}

void UInventoryComponent::OnRep_UsingWeaponSlot()
{
	if (IsValid(CharacterOwner) == false)
		return;
	
	if (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController()))
		{
			UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
			if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
				HUDWidget->NoticeWidget->RefreshAlpha();
		}
	}
}

void UInventoryComponent::Server_RequestMoveConsumable_Implementation(EInventorySlotType From, uint32 SlotID, const FItemObject& NewItemObject)
{
	if (From == EInventorySlotType::Near)
	{
		if (SlotID >= (uint32)NearWorldItems.Num())
			return;
		
		AWorldItem* WorldItem = NearWorldItems[SlotID];
		if (IsValid(WorldItem) == false || WorldItem->GetItemObject() != NewItemObject)
			return;

		if (IsValid(GameInstance) == false)
			return;
		
		const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(WorldItem->GetItemObject().ItemID);
		if (ItemInfo == nullptr || ItemInfo->ItemType != EItemType::Consumable)
			return;

		AddConsumable(WorldItem->GetItemObject());
		Client_AddConsumable(WorldItem->GetItemObject());

		NearWorldItems.RemoveAt(SlotID);
		WorldItem->Destroy();

		TArray<FItemObject> NearItems;
		for (int i = 0; i < NearWorldItems.Num(); i++)
		{
			AWorldItem* NearWorldItem = NearWorldItems[i];
			if (IsValid(NearWorldItem))
				NearItems.Emplace(NearWorldItem->GetItemObject());
		}
		Client_RefreshSlotUI(NearItems);
	}
	else if (From == EInventorySlotType::Inventory)
	{
		if (SlotID >= (uint32)InventoryItems.Num())
			return;
		
		if (InventoryItems[SlotID] != NewItemObject)
			return;

		if (IsValid(GameInstance) == false)
			return;
		
		const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(InventoryItems[SlotID].ItemID);
		if (ItemInfo == nullptr || ItemInfo->ItemType != EItemType::Consumable)
			return;
		
		AWorldItem* WorldItem = GetWorld()->SpawnActor<AWorldItem>(AWorldItem::StaticClass(), FindItemSpawnPos(), FRotator::ZeroRotator);
		if (IsValid(WorldItem) == false)
			return;

		WorldItem->Init(InventoryItems[SlotID]);
		WorldItem->Multicast_SetEnableCollision(false);
		NearWorldItems.Add(WorldItem);

		FItemObject ItemObject = InventoryItems[SlotID];
		int32 Amount = InventoryItems[SlotID].Amount;
		SubConsumable(SlotID, ItemObject, Amount);
		Client_SubConsumable(SlotID, ItemObject, Amount);
		
		TArray<FItemObject> NearItems;
		for (int i = 0; i < NearWorldItems.Num(); i++)
		{
			AWorldItem* NearWorldItem = NearWorldItems[i];
			if (IsValid(NearWorldItem))
				NearItems.Emplace(NearWorldItem->GetItemObject());
		}
		Client_RefreshSlotUI(NearItems);
	}
}

void UInventoryComponent::Client_RefreshSlotUI_Implementation(const TArray<FItemObject>& NearItems)
{
	ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController());
	if (ControllerOwner == nullptr)
		return;

	UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
	if (IsValid(HUDWidget) && IsValid(HUDWidget->InventoryWidget))
		HUDWidget->InventoryWidget->RefreshSlotUI(NearItems, GetInventoryItems());
}

void UInventoryComponent::Client_RefreshArmorUI_Implementation()
{
	ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController());
	if (ControllerOwner == nullptr)
		return;

	UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
	if (IsValid(HUDWidget) && IsValid(HUDWidget->InventoryWidget))
		HUDWidget->InventoryWidget->RefreshArmorUI();
}

void UInventoryComponent::Client_RefreshWeaponUI_Implementation()
{
	ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController());
	if (ControllerOwner == nullptr)
		return;
	
	UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
	if (IsValid(HUDWidget) && IsValid(HUDWidget->InventoryWidget))
		HUDWidget->InventoryWidget->RefreshWeaponUI();
}

void UInventoryComponent::LineTraceTimer()
{
	if (IsValid(CharacterOwner) == false)
		return;

	ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController());
	if (ControllerOwner == nullptr)
		return;
	
	if (CharacterOwner->GetFallingState() == EFallingState::Diving || CharacterOwner->GetFallingState() == EFallingState::Glide)
		return;
	
	if (CanInteraction() == false)
	{
		UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
		if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
			HUDWidget->NoticeWidget->HideInteractionText();
		return;
	}

	if (IsValid(CharacterOwner->GetOverlappedDoor()))
		return;
	
	AWorldItem* WorldItem = FindSingleWorldItemFromLineTrace();
	if (IsValid(WorldItem))
	{
		if (IsValid(GameInstance) == false)
			return;
		
		const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(WorldItem->GetItemObject().ItemID);
		if (ItemInfo == nullptr)
			return;

		UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
		if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
		{
			FString String = FString::Printf(TEXT("'%s'을(를) 줍기"), *ItemInfo->Name.ToString());
			HUDWidget->NoticeWidget->ShowInteractionText(String);
		}
	}
	else
	{
		UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
		if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
			HUDWidget->NoticeWidget->HideInteractionText();
	}
}

AWorldItem* UInventoryComponent::FindSingleWorldItemFromLineTrace()
{
	if (IsValid(CharacterOwner) == false)
		return nullptr;
	
	FVector Start = CharacterOwner->GetCameraComponent()->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCameraComponent()->GetForwardVector() * 600.f;
	
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1,
		false, ActorsToIgnore, EDrawDebugTrace::None,HitResult, true);
	
	if (bHit)
		return Cast<AWorldItem>(HitResult.GetActor());

	return nullptr;
}

FVector UInventoryComponent::FindItemSpawnPos()
{
	if (IsValid(CharacterOwner) == false)
		return FVector::ZeroVector;
	
	FVector PlayerDirs[4] =
	{
		CharacterOwner->GetActorForwardVector(),
		CharacterOwner->GetActorRightVector(),
		CharacterOwner->GetActorForwardVector() * -1.f,
		CharacterOwner->GetActorRightVector() * -1.f
	};

	bool bFound = false;
	FVector SpawnPos = FVector::ZeroVector;
		
	for (int i = 0; i < 4; i++)
	{
		FHitResult HitResult;
		TArray<AActor*> ActorsToIgnore = { CharacterOwner };
			
		FVector PlayerPos = CharacterOwner->GetActorLocation();
		FVector TargetPos = PlayerPos + PlayerDirs[i] * 100.f;

		bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), PlayerPos, TargetPos, ETraceTypeQuery::TraceTypeQuery1,
			false, ActorsToIgnore, EDrawDebugTrace::None,HitResult, true);

		if (bHit)
			continue;

		bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), TargetPos, TargetPos + FVector::DownVector * 300.f, ETraceTypeQuery::TraceTypeQuery1,
			false, ActorsToIgnore, EDrawDebugTrace::None,HitResult, true);

		if (bHit == false)
			continue;

		bFound = true;
		SpawnPos = HitResult.ImpactPoint;
		break;
	}
		
	if (bFound == false)
	{
		bFound = true;
		SpawnPos = CharacterOwner->GetActorLocation() + FVector::DownVector * CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}

	return SpawnPos;
}

int32 UInventoryComponent::GetTotalAttack()
{
	if (UsingWeaponType == EWeaponType::None)
		return 0;

	if (IsValid(GameInstance) == false)
		return 0;
	
	const FItemBaseInfo* ItemBaseInfo = GameInstance->GetItemBaseInfo(GetUsingWeapon().ItemID);
	if (ItemBaseInfo == nullptr)
		return 0;
	
	return ItemBaseInfo->Attack;
}

int32 UInventoryComponent::GetTotalDefence()
{
	if (IsValid(GameInstance) == false)
		return 0;
	
	int32 TotalDefence = 0;
	for (const FItemObject& ItemObject : ArmorSlots)
	{
		if (ItemObject.ItemID == 0)
			continue;

		const FItemBaseInfo* ItemBaseInfo = GameInstance->GetItemBaseInfo(ItemObject.ItemID);
		if (ItemBaseInfo)
			TotalDefence += ItemBaseInfo->Defence;
	}

	return TotalDefence;
}