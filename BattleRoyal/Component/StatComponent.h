#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

class UResultWidget;
class ABattleRoyalCharacter;
class ABattleRoyalPlayerController;
class ABattleRoyalGameState;

UENUM()
enum class ECurrHPChangeType : uint8
{
	Init,
	TakeDamage,
	InstanceHeal,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYAL_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStatComponent();
	
	virtual void BeginPlay() override;
	
public:
	void TakeDamage(int32 DamageAmount);
	void InstantHeal(int32 HealAmount);
	void WaitForHeal(int32 HealAmount, float WaitTime);
	void ApplyHeal();
	void CancelHeal();

	UFUNCTION(Server, Reliable)
	void Server_CancelHeal();
	
	FORCEINLINE int32 GetCurrHP() const { return CurrHP; }
	FORCEINLINE int32 GetMaxHP() const { return MaxHP; }

private:
	UFUNCTION(Client, Reliable)
	void Client_SetCurrHP(int32 NewCurrHP, ECurrHPChangeType ChangeType);

	UFUNCTION(Client, Reliable)
	void Client_WaitForHeal(float Ratio);

	UFUNCTION(Client, Reliable)
	void Client_ApplyHeal(int32 NewCurrHP);

	UFUNCTION(Client, Reliable)
	void Client_CancelHeal();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_CancelHeal();

	UFUNCTION(Client, Reliable)
	void Client_Dead();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Dead();

private:
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> DeathMontage;

private:
	// Cache
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ABattleRoyalCharacter> CharacterOwner;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ABattleRoyalGameState> GameState;
	
private:
	UPROPERTY(VisibleAnywhere)
	int32 CurrHP;

	UPROPERTY(VisibleAnywhere)
	int32 ApplyHealAmount;
	
	FTimerHandle WaitForHealHandle;
	
	// TODO: Move to Data Sheet
	int32 MaxHP = 100;
};
