#include "BattleRoyalPlayerController.h"

#include "WeaponComponent.h"
#include "BattleRoyalCharacter.h"
#include "FlyingViewerPawn.h"
#include "BattleRoyalGameMode.h"
#include "HUDWidget.h"
#include "OptionWidget.h"
#include "InventoryWidget.h"
#include "MiniMapWidget.h"
#include "WorldMapWidget.h"
#include "NoticeWidget.h"
#include "ResultWidget.h"
#include "InventoryPreview.h"
#include "Aircraft.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ABattleRoyalPlayerController::ABattleRoyalPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_OpenOption(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_OpenOption.IA_OpenOption'"));
	if (IA_OpenOption.Succeeded())
		OpenOptionAction = IA_OpenOption.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveForward(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_MoveForward.IA_MoveForward'"));
	if (IA_MoveForward.Succeeded())
		MoveForwardAction = IA_MoveForward.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveRight(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_MoveRight.IA_MoveRight'"));
	if (IA_MoveRight.Succeeded())
		MoveRightAction = IA_MoveRight.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Look(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_Look.IA_Look'"));
	if (IA_Look.Succeeded())
		LookAction = IA_Look.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Look_Fly(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_Look_Fly.IA_Look_Fly'"));
	if (IA_Look_Fly.Succeeded())
		LookAction_Fly = IA_Look_Fly.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Crouch(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_Crouch.IA_Crouch'"));
	if (IA_Crouch.Succeeded())
		CrouchAction = IA_Crouch.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Jump(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_Jump.IA_Jump'"));
	if (IA_Jump.Succeeded())
		JumpAction = IA_Jump.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Sprint(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_Sprint.IA_Sprint'"));
	if (IA_Sprint.Succeeded())
		SprintAction = IA_Sprint.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ADS(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_ADS.IA_ADS'"));
	if (IA_ADS.Succeeded())
		ADSAction = IA_ADS.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Fire(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_Fire.IA_Fire'"));
	if (IA_Fire.Succeeded())
		FireAction = IA_Fire.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Reload(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_Reload.IA_Reload'"));
	if (IA_Reload.Succeeded())
		ReloadAction = IA_Reload.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Interaction(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_Interaction.IA_Interaction'"));
	if (IA_Interaction.Succeeded())
		InteractionAction = IA_Interaction.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Interaction_Fly(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_Interaction_Fly.IA_Interaction_Fly'"));
	if (IA_Interaction_Fly.Succeeded())
		InteractionAction_Fly = IA_Interaction_Fly.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_UseHealthKit(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_UseHealthKit.IA_UseHealthKit'"));
	if (IA_UseHealthKit.Succeeded())
		UseHealthKitAction = IA_UseHealthKit.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_OpenWorldMap(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_OpenWorldMap.IA_OpenWorldMap'"));
	if (IA_OpenWorldMap.Succeeded())
		OpenWorldMapAction = IA_OpenWorldMap.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_OpenInventory(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_OpenInventory.IA_OpenInventory'"));
	if (IA_OpenInventory.Succeeded())
		OpenInventoryAction = IA_OpenInventory.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ZoomMiniMap(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_ZoomMiniMap.IA_ZoomMiniMap'"));
	if (IA_ZoomMiniMap.Succeeded())
		ZoomMiniMapAction = IA_ZoomMiniMap.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_SelectPrimary(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_SelectPrimary.IA_SelectPrimary'"));
	if (IA_SelectPrimary.Succeeded())
		SelectPrimaryAction = IA_SelectPrimary.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_SelectSecondary(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_SelectSecondary.IA_SelectSecondary'"));
	if (IA_SelectSecondary.Succeeded())
		SelectSecondaryAction = IA_SelectSecondary.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_SelectTertiary(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_SelectTertiary.IA_SelectTertiary'"));
	if (IA_SelectTertiary.Succeeded())
		SelectTertiaryAction = IA_SelectTertiary.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_StartGame(TEXT("/Script/EnhancedInput.InputAction'/Game/BattleRoyal/Input/Actions/IA_StartGame.IA_StartGame'"));
	if (IA_StartGame.Succeeded())
		StartGameAction = IA_StartGame.Object;
	
	// UI
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_HUD(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BattleRoyal/UI/Widgets/WBP_HUD.WBP_HUD_C'"));
	if (WBP_HUD.Succeeded())
		HUDWidgetClass = WBP_HUD.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_Result(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BattleRoyal/UI/Widgets/Status/WBP_Result.WBP_Result_C'"));
	if (WBP_Result.Succeeded())
		ResultWidgetClass = WBP_Result.Class;
}

void ABattleRoyalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	Aircraft = Cast<AAircraft>(UGameplayStatics::GetActorOfClass(this, AAircraft::StaticClass()));
	
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		InventoryPreview = GetWorld()->SpawnActor<AInventoryPreview>(FVector(0.f, 0.f, 100000000.f), FRotator::ZeroRotator);
		HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass, TEXT("HUD"));
		HUDWidget->AddToViewport();
	
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(this, true);
		SetShowMouseCursor(false);
	}
}

void ABattleRoyalPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleRoyalPlayerController, Nickname);
	DOREPLIFETIME(ABattleRoyalPlayerController, KillCount);
}

void ABattleRoyalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::MoveForward);
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Completed, this, &ABattleRoyalPlayerController::MoveForward);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::MoveRight);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Completed, this, &ABattleRoyalPlayerController::MoveRight);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::Look);
		EnhancedInputComponent->BindAction(LookAction_Fly, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::Look_Fly);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::ToggleCrouch);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABattleRoyalPlayerController::StopJump);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::Sprint);

		EnhancedInputComponent->BindAction(ADSAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::ADS);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::Fire);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::Reload);
		
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::Interaction);
		EnhancedInputComponent->BindAction(InteractionAction_Fly, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::Interaction_Fly);
		EnhancedInputComponent->BindAction(UseHealthKitAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::UseHealthKit);

		EnhancedInputComponent->BindAction(OpenWorldMapAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::OpenWorldMap);
		EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::OpenInventory);
		EnhancedInputComponent->BindAction(OpenOptionAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::OpenOption);
		EnhancedInputComponent->BindAction(ZoomMiniMapAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::ZoomMiniMap);
		
		EnhancedInputComponent->BindAction(SelectPrimaryAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::SelectPrimary);
		EnhancedInputComponent->BindAction(SelectSecondaryAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::SelectSecondary);
		EnhancedInputComponent->BindAction(SelectTertiaryAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::SelectTertiary);
		
		EnhancedInputComponent->BindAction(StartGameAction, ETriggerEvent::Triggered, this, &ABattleRoyalPlayerController::Server_StartGame);
	}
}

void ABattleRoyalPlayerController::Client_OpenInventory_Implementation(const TArray<FItemObject>& NearItems)
{
	ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter());
	if (BattleCharacter == nullptr)
		return;
	
	HUDWidget->InventoryWidget->RefreshNicknameUI(Nickname);
	HUDWidget->InventoryWidget->RefreshSlotUI(NearItems, BattleCharacter->GetInventoryComponent()->GetInventoryItems());
	HUDWidget->InventoryWidget->RefreshArmorUI();
	HUDWidget->InventoryWidget->RefreshWeaponUI();
	
	HUDWidget->OptionWidget->SetVisibility(ESlateVisibility::Hidden);
	HUDWidget->WorldMapWidget->SetVisibility(ESlateVisibility::Hidden);
	HUDWidget->InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, HUDWidget->InventoryWidget, EMouseLockMode::DoNotLock, true);
	SetShowMouseCursor(true);
}

void ABattleRoyalPlayerController::Client_ShowNoticeText_Implementation(const FString& NoticeString, bool bPersistent)
{
	if (IsValid(HUDWidget))
		HUDWidget->NoticeWidget->ShowNoticeText(NoticeString, bPersistent);
}

void ABattleRoyalPlayerController::Client_ShowResultUI_Implementation(bool bWinner, int32 Rank, int32 TotalPlayer)
{
	ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter());
	if (BattleCharacter == nullptr)
		return;

	BattleCharacter->GetWeaponComponent()->StopFire();
	
	HUDWidget->RemoveFromParent();
	
	ResultWidget = CreateWidget<UResultWidget>(this, ResultWidgetClass, TEXT("Result"));
	ResultWidget->RefreshUI(Nickname, bWinner, KillCount, Rank, TotalPlayer);
	ResultWidget->AddToViewport(0);
	
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, ResultWidget, EMouseLockMode::DoNotLock, true);
	SetShowMouseCursor(true);
}

void ABattleRoyalPlayerController::Client_GoToLobby_Implementation()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("TitleMap"));
}

void ABattleRoyalPlayerController::Server_RequestDive_Implementation()
{
	if (bIsDived)
		return;
	
	AFlyingViewerPawn* FlyingViewerPawn = Cast<AFlyingViewerPawn>(GetPawn());
	if (FlyingViewerPawn == nullptr)
		return;

	bIsDived = true;

	FActorSpawnParameters Parameters;
	Parameters.Owner = this;
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FVector CenterPos = FlyingViewerPawn->GetActorLocation() + FVector::DownVector * 1000.f;
	FVector SpawnPos = UKismetMathLibrary::RandomPointInBoundingBox(CenterPos, FVector(1000, 1000, 500));
	ABattleRoyalCharacter* BattleRoyalCharacter = GetWorld()->SpawnActor<ABattleRoyalCharacter>(SpawnPos, FRotator::ZeroRotator, Parameters);
	
	UnPossess();
	FlyingViewerPawn->Destroy();
	FlyingViewerPawn = nullptr;

	Possess(BattleRoyalCharacter);
	BattleRoyalCharacter->Multicast_SetFallingState(EFallingState::Diving);
}

void ABattleRoyalPlayerController::OnDead()
{
	if (ABattleRoyalGameMode* GameMode = Cast<ABattleRoyalGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		GameMode->OnDead(this);
}

void ABattleRoyalPlayerController::RideInAircraft()
{
	if (GetLocalRole() != ROLE_Authority)
		return;
	
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
	{
		UnPossess();
		BattleCharacter->Destroy();
		BattleCharacter = nullptr;
	}

	FActorSpawnParameters Parameters;
	Parameters.Owner = this;
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AFlyingViewerPawn* FlyingViewerPawn = GetWorld()->SpawnActor<AFlyingViewerPawn>(Aircraft->GetActorLocation(), Aircraft->GetActorRotation(), Parameters);
	if (FlyingViewerPawn == nullptr)
		return;
	
	Possess(FlyingViewerPawn);

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, false);
	FlyingViewerPawn->AttachToActor(Aircraft, Rules);
}


void ABattleRoyalPlayerController::MoveForward(const FInputActionValue& Value)
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->MoveForward(Value);
}

void ABattleRoyalPlayerController::MoveRight(const FInputActionValue& Value)
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->MoveRight(Value);
}

void ABattleRoyalPlayerController::Look(const FInputActionValue& Value)
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->Look(Value);
}

void ABattleRoyalPlayerController::Look_Fly(const FInputActionValue& Value)
{
	if (AFlyingViewerPawn* FlyingPawn = Cast<AFlyingViewerPawn>(GetPawn()))
		FlyingPawn->Look(Value);
}

void ABattleRoyalPlayerController::ToggleCrouch()
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->ToggleCrouch();
}

void ABattleRoyalPlayerController::StartJump()
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->StartJump();
}

void ABattleRoyalPlayerController::StopJump()
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->StopJump();
}

void ABattleRoyalPlayerController::Sprint(const FInputActionValue& Value)
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->Sprint(Value);
}

void ABattleRoyalPlayerController::ADS(const FInputActionValue& Value)
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->ADS(Value);
}

void ABattleRoyalPlayerController::Fire(const FInputActionValue& Value)
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->Fire(Value);
}

void ABattleRoyalPlayerController::Reload()
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->Reload();
}

void ABattleRoyalPlayerController::SelectPrimary()
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->SelectPrimary();
}

void ABattleRoyalPlayerController::SelectSecondary()
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->SelectSecondary();
}

void ABattleRoyalPlayerController::SelectTertiary()
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->SelectTertiary();
}

void ABattleRoyalPlayerController::Interaction()
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->Interaction();
}

void ABattleRoyalPlayerController::Interaction_Fly()
{
	if (AFlyingViewerPawn* FlyingPawn = Cast<AFlyingViewerPawn>(GetPawn()))
		FlyingPawn->Interaction();
}

void ABattleRoyalPlayerController::UseHealthKit()
{
	if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
		BattleCharacter->UseHealthKit();
}

void ABattleRoyalPlayerController::OpenWorldMap()
{
	if (HUDWidget->WorldMapWidget->GetVisibility() == ESlateVisibility::Hidden)
	{
		if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
			BattleCharacter->GetWeaponComponent()->StopFire();
		
		HUDWidget->OptionWidget->SetVisibility(ESlateVisibility::Hidden);
		HUDWidget->InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		HUDWidget->WorldMapWidget->SetVisibility(ESlateVisibility::Visible);
		
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, HUDWidget->WorldMapWidget, EMouseLockMode::DoNotLock, true);
		SetShowMouseCursor(true);
	}
}

void ABattleRoyalPlayerController::OpenInventory()
{
	ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter());
	if (BattleCharacter == nullptr)
		return;
	
	if (HUDWidget->InventoryWidget->GetVisibility() == ESlateVisibility::Hidden)
	{
		if (BattleCharacter->CanOpenUI() == false || BattleCharacter->GetMovementComponent()->IsFalling())
			return;
		
		BattleCharacter->GetInventoryComponent()->Server_SearchNearItems();
	}
}

void ABattleRoyalPlayerController::OpenOption()
{
	if (HUDWidget->OptionWidget->GetVisibility() == ESlateVisibility::Hidden)
	{
		if (ABattleRoyalCharacter* BattleCharacter = Cast<ABattleRoyalCharacter>(GetCharacter()))
			BattleCharacter->GetWeaponComponent()->StopFire();
		
		HUDWidget->InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		HUDWidget->WorldMapWidget->SetVisibility(ESlateVisibility::Hidden);
		HUDWidget->OptionWidget->SetVisibility(ESlateVisibility::Visible);
		
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, HUDWidget->OptionWidget, EMouseLockMode::DoNotLock, true);
		SetShowMouseCursor(true);
	}
}

void ABattleRoyalPlayerController::ZoomMiniMap(const FInputActionValue& Value)
{
	float ZoomValue = Value.Get<float>();
	
	if (ZoomValue == 1.0f)
		HUDWidget->MiniMapWidget->ZoomIn();
	else if (ZoomValue == -1.0f)
		HUDWidget->MiniMapWidget->ZoomOut();
}

void ABattleRoyalPlayerController::Server_StartGame_Implementation()
{
	if (ABattleRoyalGameMode* GameMode = Cast<ABattleRoyalGameMode>(UGameplayStatics::GetGameMode(this)))
		GameMode->bIsStarted = true;
}
