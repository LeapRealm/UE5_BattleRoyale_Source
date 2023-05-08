#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "BattleRoyalPlayerController.generated.h"

class AAircraft;
class AFlyingViewerPawn;
class UResultWidget;
class AInventoryPreview;
class UUserWidget;
class UHUDWidget;
class ABattleRoyalCharacter;
class UInputAction;
class UInputMappingContext;

UCLASS()
class BATTLEROYAL_API ABattleRoyalPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABattleRoyalPlayerController();
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	UFUNCTION(Client, Reliable)
	void Client_OpenInventory(const TArray<FItemObject>& NearItems);
	
	UFUNCTION(Client, Reliable)
	void Client_ShowNoticeText(const FString& NoticeString, bool bPersistent = false);
	
	UFUNCTION(Client, Reliable)
	void Client_ShowResultUI(bool bWinner, int32 Rank, int32 TotalPlayer);

	UFUNCTION(Client, Reliable)
	void Client_GoToLobby();

	UFUNCTION(Server, Reliable)
	void Server_RequestDive();
	
	void OnDead();
	void RideInAircraft();
	
private:
	void MoveForward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Look_Fly(const FInputActionValue& Value);

	void ToggleCrouch();
	void StartJump();
	void StopJump();
	void Sprint(const FInputActionValue& Value);

	void ADS(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);
	void Reload();

	void SelectPrimary();
	void SelectSecondary();
	void SelectTertiary();
	
	void Interaction();
	void Interaction_Fly();
	void UseHealthKit();
	
	void OpenWorldMap();
	void OpenInventory();
	void OpenOption();
	void ZoomMiniMap(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_StartGame();
	
public:
	FORCEINLINE UHUDWidget* GetHUDWidget() const { return HUDWidget; }
	FORCEINLINE AInventoryPreview* GetInventoryPreview() const { return InventoryPreview; }
	FORCEINLINE AAircraft* GetAircraft() const { return Aircraft; }

	FORCEINLINE bool GetIsDived() const { return bIsDived; }
	FORCEINLINE int32 GetKillCount() const { return KillCount; }

	FORCEINLINE void SetKillCount(int32 NewKillCount) { KillCount = NewKillCount; }
	FORCEINLINE void SetNickname(const FText& NewNickname) { Nickname = NewNickname; }
	
private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> OpenOptionAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveForwardAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveRightAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookAction_Fly;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ADSAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ReloadAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> InteractionAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> InteractionAction_Fly;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> UseHealthKitAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> OpenWorldMapAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> OpenInventoryAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ZoomMiniMapAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SelectPrimaryAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SelectSecondaryAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SelectTertiaryAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> StartGameAction;
	
	// UI
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	
	UPROPERTY(VisibleAnywhere, Category="UI")
	TObjectPtr<UHUDWidget> HUDWidget;
	
	UPROPERTY(VisibleAnywhere, Category="UI")
	TObjectPtr<AInventoryPreview> InventoryPreview;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> ResultWidgetClass;

	UPROPERTY(VisibleAnywhere, Category="UI")
	TObjectPtr<UResultWidget> ResultWidget;

	// World
	UPROPERTY(VisibleAnywhere, Category="World")
	TObjectPtr<AAircraft> Aircraft;

private:
	UPROPERTY(Replicated)
	FText Nickname;

	UPROPERTY(Replicated)
	int32 KillCount;

	UPROPERTY(VisibleAnywhere)
	uint8 bIsDived:1;
};
