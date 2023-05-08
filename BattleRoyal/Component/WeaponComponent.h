#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class ABattleRoyalGameState;
class ABattleRoyalCharacter;
class ABattleRoyalPlayerController;
class UBattleRoyalGameInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYAL_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponComponent();
	
	virtual void BeginPlay() override;

public:
	void StartFire();
	void StopFire();

	void RequestFire();
	void RequestReload();
	
private:
	UFUNCTION(Server, Reliable)
	void Server_TryFire();

	void LineTrace(FVector StartLocation, FVector ConeDirection, float ConeHalfAngle, float WeaponMaxDistance, bool& bOutPlayerHit, bool& bOutPlayerDead);
	void ApplyDamage(bool bIsCritical, AActor* HitActor, bool& bOutPlayerDead);
	
	UFUNCTION(Client, Reliable)
	void Client_Fire(bool bIsPlayerHit, bool bIsPlayerDead);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayFireAnim();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HitEffect(bool bIsPlayerHit, FVector ImpactPoint, FVector ImpactNormal);
	
	UFUNCTION(Server, Reliable)
	void Server_TryReload();

	UFUNCTION(Client, Reliable)
	void Client_Reload(int32 SlotIndex, int32 AddAmmo);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Reload();

	bool CanFiring();
	int32 CanReloading();

private:
	// Animation
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> BodyPistolFireMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> BodyPistolReloadMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> BodyRifleFireMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> BodyRifleReloadMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> BodyShotgunFireMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> BodyShotgunReloadMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> WeaponPistolFireMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> WeaponPistolReloadMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> WeaponRifleFireMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> WeaponRifleReloadMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> WeaponShotgunFireMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> WeaponShotgunReloadMontage;

	// Fire
	UPROPERTY(EditAnywhere, Category="Fire")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	UPROPERTY(EditAnywhere, Category="Fire")
	TObjectPtr<UNiagaraSystem> FireTrailNiagaraSystem;

	UPROPERTY(EditAnywhere, Category="Fire")
	TObjectPtr<UNiagaraSystem> ImpactDecalNiagaraSystem;

	UPROPERTY(EditAnywhere, Category="Fire")
	TObjectPtr<UNiagaraSystem> ImpactConcreteNiagaraSystem;

	UPROPERTY(EditAnywhere, Category="Fire")
	TObjectPtr<UNiagaraSystem> ImpactBloodNiagaraSystem;

	// Sound
	UPROPERTY(EditAnywhere, Category="Sound")
	TObjectPtr<USoundWave> PistolReloadSound;

	UPROPERTY(EditAnywhere, Category="Sound")
	TObjectPtr<USoundWave> RifleReloadSound;

	UPROPERTY(EditAnywhere, Category="Sound")
	TObjectPtr<USoundWave> ShotgunReloadSound;

private:
	// Cache
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ABattleRoyalCharacter> CharacterOwner;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBattleRoyalGameInstance> GameInstance;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ABattleRoyalGameState> GameState;
	
private:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<AActor>> ActorsToIgnore;
	
	FDateTime LastFiredTime;
	FTimerHandle FireTimerHandle;
};
