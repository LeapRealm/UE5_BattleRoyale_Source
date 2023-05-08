#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "WorldItem.h"
#include "BattleRoyalCharacter.generated.h"

class ADoor;
class UInputMappingContext;
class AInventoryPreview;
class UStatComponent;
class UWeaponComponent;
class UInventoryComponent;
class UCameraComponent;
class USpringArmComponent;

UENUM()
enum class EFallingState : uint8
{
	Land,
	Diving,
	Glide
};

UCLASS(config=Game)
class ABattleRoyalCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	ABattleRoyalCharacter();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
public:
	// Move Forward
	void MoveForward(const FInputActionValue& Value);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForwardValue(float NewMoveForwardValue);

	// Move Right
	void MoveRight(const FInputActionValue& Value);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRightValue(float NewMoveRightValue);
	
	// Look & Aim Pitch
	void Look(const FInputActionValue& Value);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_AimPitch(float NewAimPitch);
	
	// Crouch
	void ToggleCrouch();

	// Jump
	void StartJump();
	void StopJump();
	
	// Sprint
	void Sprint(const FInputActionValue& Value);
	
	UFUNCTION(Server, Reliable)
	void Server_Sprint(bool bWantSprint);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Sprint(bool bWantSprint);
	
	// ADS
	void ADS(const FInputActionValue& Value);
	
	UFUNCTION(Server, Reliable)
	void Server_ADS(bool bWantADS);
	
	UFUNCTION(Client, Reliable)
	void Client_ADS(bool bWantADS);

	UFUNCTION()
	void SmoothADSInterp(float Value);

	// Weapon
	void Fire(const FInputActionValue& Value);
	void Reload();

	void SelectPrimary();
	void SelectSecondary();
	void SelectTertiary();
	
	// Item
	void Interaction();
	void UseHealthKit();

	// Diving & Gliding
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetFallingState(EFallingState NewFallingState);

	UFUNCTION(Server, Reliable)
	void Server_RequestGlide();

	UFUNCTION(Server, Reliable)
	void Server_RequestLand();

	UFUNCTION()
	void SmoothParachuteInterp(float Value);

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:
	bool CanCrouching();
	bool CanJump();
	bool CanSprint();
	bool CanADS();
	bool CanUseHealthKit();
	bool CanOpenUI();

public:
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE UStatComponent* GetStatComponent() const { return StatComponent; }
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE UWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMeshComponent() const { return WeaponMeshComponent; }
	
	FORCEINLINE ADoor* GetOverlappedDoor() const { return OverlappedDoor; }
	FORCEINLINE UAnimMontage* GetDrinkingMontage() const { return DrinkingMontage; }
	FORCEINLINE EFallingState GetFallingState() const { return FallingState; }
	
	FORCEINLINE bool GetIsSprinting() const { return bIsSprinting; }
	FORCEINLINE bool GetIsADSing() const { return bIsADSing; }
	FORCEINLINE bool GetIsFiring() const { return bIsFiring; }
	FORCEINLINE bool GetIsReloading() const { return bIsReloading; }
	FORCEINLINE bool GetIsDead() const { return bIsDead; }
	FORCEINLINE bool GetIsHealCasting() const { return bIsHealCasting; }

	FORCEINLINE float GetMoveForwardValue() const { return MoveForwardValue; }
	FORCEINLINE float GetMoveRightValue() const { return MoveRightValue; }
	FORCEINLINE float GetCurrAimPitch() const { return CurrAimPitch; }
	
	FORCEINLINE void SetIsFiring(bool bNewIsFiring) { bIsFiring = bNewIsFiring; }
	FORCEINLINE void SetIsReloading(bool bNewIsReloading) { bIsReloading = bNewIsReloading; }
	FORCEINLINE void SetIsDead(bool bNewIsDead) { bIsDead = bNewIsDead; }
	FORCEINLINE void SetIsHealCasting(bool bNewIsHealCasting) { bIsHealCasting = bNewIsHealCasting; }
	
	void SetWeaponMesh(EWeaponType WeaponType, USkeletalMesh* NewMesh);
	void SetArmorMesh(EArmorType ArmorType, USkeletalMesh* NewMesh, USkeletalMesh* NewAttachmentMesh = nullptr);
	
private:
	// Input
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> BattleRoyalMappingContext;
	
	// Mesh
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<UStaticMeshComponent> ParachuteMeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> ShirtMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> PantsMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> BootsMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> HandsMeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> VestMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> BackpackMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> BackpackStringMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> HelmetMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> HelmetOutfitMeshComponent;

	// Default Mesh
	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMesh> DefaultHeadMesh;

	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMesh> DefaultShirtMesh;

	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMesh> DefaultHandsMesh;

	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMesh> DefaultPantsMesh;

	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMesh> DefaultBootsMesh;

	// Animation
	UPROPERTY(EditAnywhere, Category="Animation")
	TSubclassOf<UAnimInstance> AnimBPClass;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> DrinkingMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> DivingMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> GlidingMontage;
	
	// Camera
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	// Custom Component
	UPROPERTY(VisibleAnywhere, Category="Component")
	TObjectPtr<UStatComponent> StatComponent;

	UPROPERTY(VisibleAnywhere, Category="Component")
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, Category="Component")
	TObjectPtr<UWeaponComponent> WeaponComponent;

	// Timeline
	UPROPERTY(EditAnywhere, Category="Timeline")
	TObjectPtr<UCurveFloat> SmoothCurveADS;
	
	UPROPERTY(VisibleAnywhere, Category="Timeline")
	TObjectPtr<UTimelineComponent> SmoothADSTimeline;

	UPROPERTY(EditAnywhere, Category="Timeline")
	TObjectPtr<UCurveFloat> SmoothCurveParachute;
	
	UPROPERTY(VisibleAnywhere, Category="Timeline")
	TObjectPtr<UTimelineComponent> SmoothParachuteTimeline;

	// Sound
	UPROPERTY(EditAnywhere, Category="Sound")
	TObjectPtr<USoundWave> ParachuteOpeningSound;

	// Sound
	UPROPERTY(EditAnywhere, Category="Sound")
	TObjectPtr<USoundWave> ParachuteWindSound;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAudioComponent> WindAudioComponent;
	
private:
	UPROPERTY(VisibleAnywhere, Replicated)
	float MoveForwardValue;
	
	UPROPERTY(VisibleAnywhere, Replicated)
	float MoveRightValue;

	UPROPERTY(VisibleAnywhere, Replicated)
	float TargetAimPitch;
	
	UPROPERTY(VisibleAnywhere, Replicated)
	float TargetMeshRollOffset;
	
	UPROPERTY(VisibleAnywhere, Replicated)
	float TargetMeshPitchOffset;

	UPROPERTY(VisibleAnywhere)
	float PrevMoveForwardValue;

	UPROPERTY(VisibleAnywhere)
	float PrevMoveRightValue;

	UPROPERTY(VisibleAnywhere)
	float PrevAimPitch;

	UPROPERTY(VisibleAnywhere)
	float CurrAimPitch;

	UPROPERTY(VisibleAnywhere)
	float CurrMeshRollOffset;
	
	UPROPERTY(VisibleAnywhere)
	float CurrMeshPitchOffset;

	UPROPERTY(VisibleAnywhere)
	float CurrReticleSpreadFactor;

	UPROPERTY(VisibleAnywhere)
	float TargetReticleSpreadFactor;

	UPROPERTY(VisibleAnywhere)
	float TargetSpringArmZPos;

	UPROPERTY(EditAnywhere)
	float TargetSpringArmLength = 300;

	FOnTimelineFloat SmoothADSInterpFunction;
	FOnTimelineFloat SmoothParachuteInterpFunction;

private:
	UPROPERTY(VisibleAnywhere)
	uint8 bIsSprinting:1;

	UPROPERTY(VisibleAnywhere)
	uint8 bIsADSing:1;
	
	UPROPERTY(VisibleAnywhere)
	uint8 bIsFiring:1;

	UPROPERTY(VisibleAnywhere)
	uint8 bIsReloading:1;

	UPROPERTY(VisibleAnywhere)
	uint8 bIsDead:1;

	UPROPERTY(VisibleAnywhere)
	uint8 bIsHealCasting:1;
	
	// uint8 bIsSwimming:1;
	// uint8 bIsFalling:1;

	UPROPERTY(VisibleAnywhere)
	EFallingState FallingState = EFallingState::Land;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ADoor> OverlappedDoor;

public:
	// TODO: Move to DataSheet
	float DefaultCrouchSpeed = 150.f;
	float DefaultWalkSpeed = 300.f;
	float DefaultSprintSpeed = 600.f;
	float DefaultSwimSpeed = 200.f;
	float DefaultDiveSpeed = 8000.f;
	float DefaultGlideSpeed = 4000.f;
	float DefaultCrouchHalfHeight = 70.f;
	float DefaultSpringArmZPos = 166.f;
	float CrouchMoveSpringArmZPos = 140.f;
	float CrouchIdleSpringArmZPos = 105.f;
	float DefaultSpreadFactor = 2.f;
	float ADSSpreadFactor = 1.f;
};
