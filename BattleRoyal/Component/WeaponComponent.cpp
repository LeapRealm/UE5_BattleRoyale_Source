#include "WeaponComponent.h"

#include "StatComponent.h"
#include "InventoryComponent.h"
#include "BattleRoyalGameInstance.h"
#include "BattleRoyalCharacter.h"
#include "BattleRoyalGameState.h"
#include "BattleRoyalPlayerController.h"
#include "HUDWidget.h"
#include "NoticeWidget.h"
#include "ReticleWidget.h"
#include "WorldItem.h"
#include "FireCameraShake.h"

#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/DamageEvents.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	LastFiredTime = FDateTime::UtcNow();
	
	// Animation
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MM_Pistol_Fire_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Characters/Animations/Actions/Pistol/MM_Pistol_Fire_Montage.MM_Pistol_Fire_Montage'"));
	if (MM_Pistol_Fire_Montage.Succeeded())
		BodyPistolFireMontage = MM_Pistol_Fire_Montage.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MM_Pistol_Reload_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Characters/Animations/Actions/Pistol/MM_Pistol_Reload_Montage.MM_Pistol_Reload_Montage'"));
	if (MM_Pistol_Reload_Montage.Succeeded())
		BodyPistolReloadMontage = MM_Pistol_Reload_Montage.Object;
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MM_Rifle_Fire_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Characters/Animations/Actions/Rifle/MM_Rifle_Fire_Montage.MM_Rifle_Fire_Montage'"));
	if (MM_Rifle_Fire_Montage.Succeeded())
		BodyRifleFireMontage = MM_Rifle_Fire_Montage.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MM_Rifle_Reload_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Characters/Animations/Actions/Rifle/MM_Rifle_Reload_Montage.MM_Rifle_Reload_Montage'"));
	if (MM_Rifle_Reload_Montage.Succeeded())
		BodyRifleReloadMontage = MM_Rifle_Reload_Montage.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MM_Shotgun_Fire_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Characters/Animations/Actions/Shotgun/MM_Shotgun_Fire_Montage.MM_Shotgun_Fire_Montage'"));
	if (MM_Shotgun_Fire_Montage.Succeeded())
		BodyShotgunFireMontage = MM_Shotgun_Fire_Montage.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MM_Shotgun_Reload_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Characters/Animations/Actions/Shotgun/MM_Shotgun_Reload_Montage.MM_Shotgun_Reload_Montage'"));
	if (MM_Shotgun_Reload_Montage.Succeeded())
		BodyShotgunReloadMontage = MM_Shotgun_Reload_Montage.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Weap_Pistol_Fire_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Weapons/Pistol/Animations/Weap_Pistol_Fire_Montage.Weap_Pistol_Fire_Montage'"));
	if (Weap_Pistol_Fire_Montage.Succeeded())
		WeaponPistolFireMontage = Weap_Pistol_Fire_Montage.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Weap_Pistol_Reload_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Weapons/Pistol/Animations/Weap_Pistol_Reload_Montage.Weap_Pistol_Reload_Montage'"));
	if (Weap_Pistol_Reload_Montage.Succeeded())
		WeaponPistolReloadMontage = Weap_Pistol_Reload_Montage.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Weap_Rifle_Fire_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Weapons/Rifle/Animations/Weap_Rifle_Fire_Montage.Weap_Rifle_Fire_Montage'"));
	if (Weap_Rifle_Fire_Montage.Succeeded())
		WeaponRifleFireMontage = Weap_Rifle_Fire_Montage.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Weap_Rifle_Reload_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Weapons/Rifle/Animations/Weap_Rifle_Reload_Montage.Weap_Rifle_Reload_Montage'"));
	if (Weap_Rifle_Reload_Montage.Succeeded())
		WeaponRifleReloadMontage = Weap_Rifle_Reload_Montage.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Weap_Shotgun_Fire_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Weapons/Shotgun/Animations/Weap_Shotgun_Fire_Montage.Weap_Shotgun_Fire_Montage'"));
	if (Weap_Shotgun_Fire_Montage.Succeeded())
		WeaponShotgunFireMontage = Weap_Shotgun_Fire_Montage.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Weap_Shotgun_Reload_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Weapons/Shotgun/Animations/Weap_Shotgun_Reload_Montage.Weap_Shotgun_Reload_Montage'"));
	if (Weap_Shotgun_Reload_Montage.Succeeded())
		WeaponShotgunReloadMontage = Weap_Shotgun_Reload_Montage.Object;

	// Niagara System
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_WeaponFire_Tracer(TEXT("/Script/Niagara.NiagaraSystem'/Game/BattleRoyal/Particles/Weapons/NS_WeaponFire_Tracer.NS_WeaponFire_Tracer'"));
	if (NS_WeaponFire_Tracer.Succeeded())
		FireTrailNiagaraSystem = NS_WeaponFire_Tracer.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_ImpactDecals(TEXT("/Script/Niagara.NiagaraSystem'/Game/BattleRoyal/Particles/Impacts/NS_ImpactDecals.NS_ImpactDecals'"));
	if (NS_ImpactDecals.Succeeded())
		ImpactDecalNiagaraSystem = NS_ImpactDecals.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_ImpactConcrete(TEXT("/Script/Niagara.NiagaraSystem'/Game/BattleRoyal/Particles/Impacts/NS_ImpactConcrete.NS_ImpactConcrete'"));
	if (NS_ImpactConcrete.Succeeded())
		ImpactConcreteNiagaraSystem = NS_ImpactConcrete.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_ShotOnce(TEXT("/Script/Niagara.NiagaraSystem'/Game/BattleRoyal/Particles/BloodSplatter/Particles/NS_ShotOnce.NS_ShotOnce'"));
	if (NS_ShotOnce.Succeeded())
		ImpactBloodNiagaraSystem = NS_ShotOnce.Object;

	// Camera Shake
	CameraShakeClass = UFireCameraShake::StaticClass();

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundWave> SW_Pistol_Reload(TEXT("/Script/Engine.SoundWave'/Game/BattleRoyal/Audio/SW_Pistol_Reload.SW_Pistol_Reload'"));
	if (SW_Pistol_Reload.Succeeded())
		PistolReloadSound = SW_Pistol_Reload.Object;
	
	static ConstructorHelpers::FObjectFinder<USoundWave> SW_Rifle_Reload(TEXT("/Script/Engine.SoundWave'/Game/BattleRoyal/Audio/SW_Rifle_Reload.SW_Rifle_Reload'"));
	if (SW_Rifle_Reload.Succeeded())
		RifleReloadSound = SW_Rifle_Reload.Object;

	static ConstructorHelpers::FObjectFinder<USoundWave> SW_Shotgun_Reload(TEXT("/Script/Engine.SoundWave'/Game/BattleRoyal/Audio/SW_Shotgun_Reload.SW_Shotgun_Reload'"));
	if (SW_Shotgun_Reload.Succeeded())
		ShotgunReloadSound = SW_Shotgun_Reload.Object;
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterOwner = Cast<ABattleRoyalCharacter>(GetOwner());
	check(CharacterOwner);
	
	GameInstance = Cast<UBattleRoyalGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(GameInstance);
	
	GameState = Cast<ABattleRoyalGameState>(UGameplayStatics::GetGameState(this));
	check(GameState);
	
	ActorsToIgnore.Add(CharacterOwner);
}

void UWeaponComponent::StartFire()
{
	if (CanFiring() == false)
		return;

	if (IsValid(CharacterOwner) == false)
		return;
	
	CharacterOwner->SetIsFiring(true);
	Server_TryFire();
	
	FItemObject& UsingWeapon = CharacterOwner->GetInventoryComponent()->GetUsingWeapon();

	if (IsValid(GameInstance) == false)
		return;
	
	const FItemBaseInfo* WeaponInfo = GameInstance->GetItemBaseInfo(UsingWeapon.ItemID);
	if (WeaponInfo == nullptr)
		return;
	
	CharacterOwner->GetWorldTimerManager().SetTimer(FireTimerHandle, this,
		&UWeaponComponent::RequestFire, WeaponInfo->FireRate, true);
}

void UWeaponComponent::StopFire()
{
	if (IsValid(CharacterOwner) == false)
		return;
	
	CharacterOwner->SetIsFiring(false);
	
	if (FireTimerHandle.IsValid())
	{
		CharacterOwner->GetWorldTimerManager().ClearTimer(FireTimerHandle);
		FireTimerHandle.Invalidate();
	}
}

void UWeaponComponent::RequestFire()
{
	if (CanFiring() == false)
		return;
	
	Server_TryFire();
}

void UWeaponComponent::RequestReload()
{
	if (CanReloading() == -1)
		return;
	
	Server_TryReload();
}

void UWeaponComponent::Server_TryFire_Implementation()
{
	if (CanFiring() == false)
		return;

	if (IsValid(CharacterOwner) == false || IsValid(GameInstance) == false)
		return;
	
	FItemObject& UsingWeapon = CharacterOwner->GetInventoryComponent()->GetUsingWeapon();
	const FItemBaseInfo* WeaponInfo = GameInstance->GetItemBaseInfo(UsingWeapon.ItemID);
	if (WeaponInfo == nullptr)
		return;

	float ElapsedSeconds = (FDateTime::UtcNow() - LastFiredTime).GetTotalSeconds();
	if (ElapsedSeconds < (WeaponInfo->FireRate - 0.05f))
		return;

	// TODO : Fire Projectile
	// Fire LineTrace
	FVector StartLocation = CharacterOwner->GetCameraComponent()->GetComponentLocation();
	FVector ConeDirection = CharacterOwner->GetCameraComponent()->GetForwardVector();

	bool bPlayerHit = false;
	bool bPlayerDead = false;
	
	if (WeaponInfo->WeaponType == EWeaponType::Pistol)
	{
		float WeaponMaxDistance = 5000.f;
		float ConeHalfAngle = (CharacterOwner->GetIsADSing()) ? 0.25f : 1.5f;
		LineTrace(StartLocation, ConeDirection, ConeHalfAngle, WeaponMaxDistance, bPlayerHit, bPlayerDead);
	}
	else if (WeaponInfo->WeaponType == EWeaponType::Rifle)
	{
		float WeaponMaxDistance = 10000.f;
		float ConeHalfAngle = (CharacterOwner->GetIsADSing()) ? 0.25f : 2.5f;
		LineTrace(StartLocation, ConeDirection, ConeHalfAngle, WeaponMaxDistance, bPlayerHit, bPlayerDead);
	}
	else if (WeaponInfo->WeaponType == EWeaponType::Shotgun)
	{
		for (int i = 0; i < 6; i++)
		{
			float WeaponMaxDistance = 1500.f;
			float ConeHalfAngle = (CharacterOwner->GetIsADSing()) ? 1.f : 3.f;
			LineTrace(StartLocation, ConeDirection, ConeHalfAngle, WeaponMaxDistance, bPlayerHit, bPlayerDead);
		}
	}
	
	UsingWeapon.MagazineAmmoAmount--;
	LastFiredTime = FDateTime::UtcNow();

	Client_Fire(bPlayerHit, bPlayerDead);
	Multicast_PlayFireAnim();
}

void UWeaponComponent::LineTrace(FVector StartLocation, FVector ConeDirection, float ConeHalfAngle, float WeaponMaxDistance, bool& bOutPlayerHit, bool& bOutPlayerDead)
{
	if (IsValid(CharacterOwner) == false || CharacterOwner->HasAuthority() == false)
		return;
	
	FHitResult HitResult;
	FVector EndLocation = StartLocation + UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ConeDirection, ConeHalfAngle) * WeaponMaxDistance;
		
	bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, EndLocation, TraceTypeQuery1,
		true, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	if (bHit)
	{
		bool bPlayer = HitResult.GetActor()->GetClass() == ABattleRoyalCharacter::StaticClass();
		if (bPlayer)
		{
			bOutPlayerHit = true;

			bool IsCritical = (HitResult.BoneName == TEXT("head"));
			ApplyDamage(IsCritical, HitResult.GetActor(), bOutPlayerDead);
		}
		Multicast_HitEffect(bPlayer, HitResult.ImpactPoint, HitResult.ImpactNormal);
	}
}

void UWeaponComponent::ApplyDamage(bool bIsCritical, AActor* HitActor, bool& bOutPlayerDead)
{
	if (IsValid(GameState) == false || GameState->bCanDamaged == false)
		return;
	
	if (IsValid(CharacterOwner) == false || CharacterOwner->HasAuthority() == false)
		return;

	ABattleRoyalCharacter* HitCharacter = Cast<ABattleRoyalCharacter>(HitActor);
	if (IsValid(HitCharacter) == false)
		return;
	
	int32 Attack = CharacterOwner->GetInventoryComponent()->GetTotalAttack();
	int32 Defence = HitCharacter->GetInventoryComponent()->GetTotalDefence();
	int32 Damage = bIsCritical ? FMath::Max(FMath::FloorToInt((Attack - (Defence / 10.f)) * 1.5f), 0) :
								FMath::Max(FMath::FloorToInt(Attack - (Defence / 10.f)), 0);

	if (HitCharacter->GetStatComponent()->GetCurrHP() <= Damage)
	{
		bOutPlayerDead = true;
		
		if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController()))
			ControllerOwner->SetKillCount(ControllerOwner->GetKillCount() + 1);
	}
	
	UGameplayStatics::ApplyDamage(HitCharacter, Damage, CharacterOwner->GetController(),CharacterOwner, nullptr);
}

void UWeaponComponent::Client_Fire_Implementation(bool bIsPlayerHit, bool bIsPlayerDead)
{
	if (IsValid(CharacterOwner) == false)
		return;
	
	ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController());
	if (ControllerOwner == nullptr)
		return;

	UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
	if (IsValid(HUDWidget))
	{
		if (bIsPlayerHit)
		{
			switch (CharacterOwner->GetInventoryComponent()->GetUsingWeaponType())
			{
			case EWeaponType::Pistol:	HUDWidget->ReticlePistolWidget->PlayHitMarker();	break;
			case EWeaponType::Rifle:	HUDWidget->ReticleRifleWidget->PlayHitMarker();		break;
			case EWeaponType::Shotgun:	HUDWidget->ReticleShotgunWidget->PlayHitMarker();	break;
			}
		}

		if (bIsPlayerDead)
		{
			switch (CharacterOwner->GetInventoryComponent()->GetUsingWeaponType())
			{
			case EWeaponType::Pistol:	HUDWidget->ReticlePistolWidget->PlayKillMarker();	break;
			case EWeaponType::Rifle:	HUDWidget->ReticleRifleWidget->PlayKillMarker();	break;
			case EWeaponType::Shotgun:	HUDWidget->ReticleShotgunWidget->PlayKillMarker();	break;
			}
		}
	}
	
	if (APlayerCameraManager* CameraManager = ControllerOwner->PlayerCameraManager)
		CameraManager->StartCameraShake(CameraShakeClass);

	FItemObject& UsingWeapon = CharacterOwner->GetInventoryComponent()->GetUsingWeapon();
	UsingWeapon.MagazineAmmoAmount--;
}

void UWeaponComponent::Multicast_PlayFireAnim_Implementation()
{
	if (IsValid(CharacterOwner) == false)
		return;
	
	EWeaponType WeaponType = CharacterOwner->GetInventoryComponent()->GetUsingWeaponType();
	
	switch (WeaponType)
	{
	case EWeaponType::Pistol:
		CharacterOwner->PlayAnimMontage(BodyPistolFireMontage);
		CharacterOwner->GetWeaponMeshComponent()->PlayAnimation(WeaponPistolFireMontage, false);
		break;
		
	case EWeaponType::Rifle:
		CharacterOwner->PlayAnimMontage(BodyRifleFireMontage);
		CharacterOwner->GetWeaponMeshComponent()->PlayAnimation(WeaponRifleFireMontage, false);
		break;
		
	case EWeaponType::Shotgun:
		CharacterOwner->PlayAnimMontage(BodyShotgunFireMontage);
		CharacterOwner->GetWeaponMeshComponent()->PlayAnimation(WeaponShotgunFireMontage, false);
		break;
	}
}

void UWeaponComponent::Multicast_HitEffect_Implementation(bool bIsPlayerHit, FVector ImpactPoint, FVector ImpactNormal)
{
	if (IsValid(CharacterOwner) == false || CharacterOwner->HasAuthority())
		return;
	
	// Trail Niagara Effect
	FVector Location = CharacterOwner->GetWeaponMeshComponent()->GetSocketLocation(TEXT("Muzzle"));
	FRotator Rotation = CharacterOwner->GetWeaponMeshComponent()->GetSocketRotation(TEXT("Muzzle"));
	UNiagaraComponent* FireTrailNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(FireTrailNiagaraSystem, CharacterOwner->GetWeaponMeshComponent(),TEXT("None"),
		Location, Rotation, EAttachLocation::KeepWorldPosition, true, true, ENCPoolMethod::AutoRelease);

	TArray<FVector> ImpactPositions;
	ImpactPositions.Emplace(ImpactPoint);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(FireTrailNiagaraComponent, TEXT("User.ImpactPositions"), ImpactPositions);

	if (FireTrailNiagaraComponent)
	{
		FireTrailNiagaraComponent->SetVariablePosition(TEXT("User.MuzzlePosition"), CharacterOwner->GetWeaponMeshComponent()->GetSocketLocation(TEXT("Muzzle")));
		FireTrailNiagaraComponent->SetVariableBool(TEXT("User.Trigger"), true);
	}

	if (bIsPlayerHit)
	{
		// Impact Blood Niagara Effect
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactBloodNiagaraSystem,
			ImpactPoint, ImpactNormal.Rotation(), FVector::OneVector, true, true, ENCPoolMethod::AutoRelease);
	}
	else
	{
		// Impact Decal Niagara Effect
		UNiagaraComponent* DecalNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactDecalNiagaraSystem,
			ImpactPoint, FRotator::ZeroRotator, FVector::OneVector, true, true, ENCPoolMethod::AutoRelease);

		if (DecalNiagaraComponent)
			DecalNiagaraComponent->SetVariableBool(TEXT("User.Trigger"), true);
	
		TArray<int32> ImpactSurfaces;
		ImpactSurfaces.Emplace(2);
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayInt32(DecalNiagaraComponent, TEXT("User.ImpactSurfaces"), ImpactSurfaces);
	
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(DecalNiagaraComponent, TEXT("User.ImpactPositions"), ImpactPositions);

		TArray<FVector> ImpactNormals;
		ImpactNormals.Emplace(ImpactNormal);
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(DecalNiagaraComponent, TEXT("User.ImpactNormals"), ImpactNormals);

		if (DecalNiagaraComponent)
			DecalNiagaraComponent->SetVariableInt(TEXT("User.NumberOfHits"), 1);
	
		// Impact Concrete Niagara Effect
		UNiagaraComponent* ImpactConcreteNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactConcreteNiagaraSystem,
			ImpactPoint, FRotator::ZeroRotator, FVector::OneVector, true, true, ENCPoolMethod::AutoRelease);
	
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(ImpactConcreteNiagaraComponent, TEXT("User.ImpactNormals"), ImpactNormals);
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(ImpactConcreteNiagaraComponent, TEXT("User.ImpactPositions"), ImpactPositions);

		if (ImpactConcreteNiagaraComponent)
		{
			ImpactConcreteNiagaraComponent->SetWorldLocation(ImpactPoint);
			ImpactConcreteNiagaraComponent->SetVariableInt(TEXT("User.StartOffset"), 0);
			ImpactConcreteNiagaraComponent->SetVariableInt(TEXT("User.NumberOfHits"), 1);
			ImpactConcreteNiagaraComponent->SetVariablePosition(TEXT("User.MuzzlePosition"), CharacterOwner->GetWeaponMeshComponent()->GetSocketLocation(TEXT("Muzzle")));
			ImpactConcreteNiagaraComponent->SetTickBehavior(ENiagaraTickBehavior::UseComponentTickGroup);
		}
	}
}

void UWeaponComponent::Server_TryReload_Implementation()
{
	int32 SlotIndex = CanReloading();
	if (SlotIndex == -1)
		return;

	if (IsValid(CharacterOwner) == false || IsValid(GameInstance) == false)
		return;
	
	FItemObject& UsingWeapon = CharacterOwner->GetInventoryComponent()->GetUsingWeapon();
	const FItemBaseInfo* WeaponInfo = GameInstance->GetItemBaseInfo(UsingWeapon.ItemID);
	if (WeaponInfo == nullptr)
		return;
	
	TArray<FItemObject>& ItemObjects = CharacterOwner->GetInventoryComponent()->GetInventoryItems();
	
	int32 NeedAmmo = WeaponInfo->MaxMagazineAmmoAmount - UsingWeapon.MagazineAmmoAmount;
	int32 HaveAmmo = ItemObjects[SlotIndex].Amount;
	int32 AddAmmo = (NeedAmmo >= HaveAmmo) ? HaveAmmo : NeedAmmo;
	
	UsingWeapon.MagazineAmmoAmount += AddAmmo;
	ItemObjects[SlotIndex].Amount -= AddAmmo;
	if (ItemObjects[SlotIndex].Amount <= 0)
		ItemObjects.RemoveAt(SlotIndex);
	
	Client_Reload(SlotIndex, AddAmmo);
	Multicast_Reload();
}

void UWeaponComponent::Client_Reload_Implementation(int32 SlotIndex, int32 AddAmmo)
{
	if (IsValid(CharacterOwner) == false || IsValid(GameInstance) == false)
		return;
	
	FItemObject& UsingWeapon = CharacterOwner->GetInventoryComponent()->GetUsingWeapon();
	
	TArray<FItemObject>& ItemObjects = CharacterOwner->GetInventoryComponent()->GetInventoryItems();
	const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(ItemObjects[SlotIndex].ItemID);
	if (ItemInfo == nullptr)
		return;

	EWeaponType WeaponType = CharacterOwner->GetInventoryComponent()->GetUsingWeaponType();
	if (WeaponType == EWeaponType::Pistol)
	{
		if (ItemInfo->ConsumableType != EConsumableType::AmmoPistol)
			return;
	}
	else if (WeaponType == EWeaponType::Rifle)
	{
		if (ItemInfo->ConsumableType != EConsumableType::AmmoRifle)
			return;
	}
	else if (WeaponType == EWeaponType::Shotgun)
	{
		if (ItemInfo->ConsumableType != EConsumableType::AmmoShotgun)
			return;
	}

	if (ItemObjects[SlotIndex].Amount < AddAmmo)
		return;
	
	UsingWeapon.MagazineAmmoAmount += AddAmmo;
	ItemObjects[SlotIndex].Amount -= AddAmmo;
	if (ItemObjects[SlotIndex].Amount <= 0)
		ItemObjects.RemoveAt(SlotIndex);
	
	switch (WeaponType)
	{
	case EWeaponType::Pistol:
		UGameplayStatics::PlaySound2D(this, PistolReloadSound);
		break;
		
	case EWeaponType::Rifle:
		UGameplayStatics::PlaySound2D(this, RifleReloadSound);
		break;
		
	case EWeaponType::Shotgun:
		UGameplayStatics::PlaySound2D(this, ShotgunReloadSound);
		break;
	}
}

void UWeaponComponent::Multicast_Reload_Implementation()
{
	if (IsValid(CharacterOwner) == false)
		return;
	
	CharacterOwner->SetIsReloading(true);
	
	EWeaponType WeaponType = CharacterOwner->GetInventoryComponent()->GetUsingWeaponType();
	float AnimTime = 0.f;
	
	switch (WeaponType)
	{
	case EWeaponType::Pistol:
		AnimTime = CharacterOwner->PlayAnimMontage(BodyPistolReloadMontage);
		CharacterOwner->GetWeaponMeshComponent()->PlayAnimation(WeaponPistolReloadMontage, false);
		break;
		
	case EWeaponType::Rifle:
		AnimTime = CharacterOwner->PlayAnimMontage(BodyRifleReloadMontage);
		CharacterOwner->GetWeaponMeshComponent()->PlayAnimation(WeaponRifleReloadMontage, false);
		break;
		
	case EWeaponType::Shotgun:
		AnimTime = CharacterOwner->PlayAnimMontage(BodyShotgunReloadMontage);
		CharacterOwner->GetWeaponMeshComponent()->PlayAnimation(WeaponShotgunReloadMontage, false);
		break;
	}

	FTimerHandle TimerHandle;
	CharacterOwner->GetWorldTimerManager().SetTimer(TimerHandle, [this]()
	{
		if (IsValid(CharacterOwner))
			CharacterOwner->SetIsReloading(false);
	}, AnimTime, false);
}

bool UWeaponComponent::CanFiring()
{
	if (IsValid(CharacterOwner) == false)
		return false;
	
	if (CharacterOwner->GetIsSprinting() || CharacterOwner->GetIsReloading() || CharacterOwner->GetIsDead() || CharacterOwner->GetIsHealCasting() ||
		CharacterOwner->GetCharacterMovement()->IsSwimming() || CharacterOwner->GetCharacterMovement()->IsFalling())
			return false;
	
	const FItemObject& UsingWeapon = CharacterOwner->GetInventoryComponent()->GetUsingWeapon();
	if (UsingWeapon.ItemID == 0)
		return false;

	if (UsingWeapon.MagazineAmmoAmount <= 0)
	{
		if (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy)
		{
			if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController()))
			{
				UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
				if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
					HUDWidget->NoticeWidget->ShowNoticeText(TEXT("탄창에 탄약이 없습니다"));
			}
		}
		return false;
	}
	
	return true;
}

int32 UWeaponComponent::CanReloading()
{
	if (IsValid(CharacterOwner) == false)
		return -1;
	
	if (CharacterOwner->GetIsSprinting() || CharacterOwner->GetIsFiring() || CharacterOwner->GetIsReloading() || CharacterOwner->GetIsDead() || CharacterOwner->GetIsHealCasting() ||
		CharacterOwner->GetCharacterMovement()->IsSwimming() || CharacterOwner->GetCharacterMovement()->IsFalling())
		return -1;
	
	const FItemObject& UsingWeapon = CharacterOwner->GetInventoryComponent()->GetUsingWeapon();
	if (UsingWeapon.ItemID == 0)
		return -1;

	if (IsValid(GameInstance) == false)
		return -1;
	
	const FItemBaseInfo* WeaponInfo = GameInstance->GetItemBaseInfo(UsingWeapon.ItemID);
	if (WeaponInfo == nullptr)
		return -1;
	
	if (UsingWeapon.MagazineAmmoAmount >= WeaponInfo->MaxMagazineAmmoAmount)
	{
		if (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy)
		{
			if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController()))
			{
				UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
				if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
					HUDWidget->NoticeWidget->ShowNoticeText(TEXT("이미 탄창이 가득 찼습니다"));
			}
		}
		return -1;
	}
	
	const TArray<FItemObject>& ItemObjects = CharacterOwner->GetInventoryComponent()->GetInventoryItems();
	for (int i = 0; i < ItemObjects.Num(); i++)
	{
		const FItemBaseInfo* ItemInfo = GameInstance->GetItemBaseInfo(ItemObjects[i].ItemID);
		if (ItemInfo == nullptr)
			return -1;

		if (WeaponInfo->WeaponType == EWeaponType::Pistol)
		{
			if (ItemInfo->ConsumableType == EConsumableType::AmmoPistol)
				return i;
		}
		else if (WeaponInfo->WeaponType == EWeaponType::Rifle)
		{
			if (ItemInfo->ConsumableType == EConsumableType::AmmoRifle)
				return i;
		}
		else if (WeaponInfo->WeaponType == EWeaponType::Shotgun)
		{
			if (ItemInfo->ConsumableType == EConsumableType::AmmoShotgun)
				return i;
		}
	}

	if (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(CharacterOwner->GetController()))
		{
			UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
			if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
				HUDWidget->NoticeWidget->ShowNoticeText(TEXT("인벤토리에 탄약이 없습니다"));
		}
	}
	
	return -1;
}
