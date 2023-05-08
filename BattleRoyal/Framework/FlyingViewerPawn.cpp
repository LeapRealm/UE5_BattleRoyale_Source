#include "FlyingViewerPawn.h"

#include "BattleRoyalPlayerController.h"
#include "HUDWidget.h"
#include "InventoryWidget.h"
#include "WorldMapWidget.h"
#include "NoticeWidget.h"
#include "InventoryPreview.h"

#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/SlateWrapperTypes.h"
#include "GameFramework/SpringArmComponent.h"

AFlyingViewerPawn::AFlyingViewerPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	bOnlyRelevantToOwner = true;
	
	// Input
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_FlyingViewer(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/BattleRoyal/Input/IMC_FlyingViewer.IMC_FlyingViewer'"));
	if (IMC_FlyingViewer.Succeeded())
		FlyingViewerMappingContext = IMC_FlyingViewer.Object;

	// Camera
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	RootComponent = SpringArmComponent;
	SpringArmComponent->TargetArmLength = 3000.f;
	SpringArmComponent->bUsePawnControlRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

void AFlyingViewerPawn::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(false);

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
		{
			ControllerOwner->GetInventoryPreview()->ResetMeshes();
			ControllerOwner->GetHUDWidget()->ShowUnarmedReticle();
			
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ControllerOwner->GetLocalPlayer()))
			{
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(FlyingViewerMappingContext, 0);
			}

			UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
			if (IsValid(HUDWidget))
			{
				HUDWidget->WorldMapWidget->ShowAircraftUI();
				HUDWidget->NoticeWidget->ShowInteractionText(TEXT("다이빙하기"));
				
				if (HUDWidget->InventoryWidget->GetVisibility() == ESlateVisibility::Visible)
				{
					HUDWidget->InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
					UWidgetBlueprintLibrary::SetInputMode_GameOnly(ControllerOwner, true);
					ControllerOwner->SetShowMouseCursor(false);
				}
			}
		}
	}
}

void AFlyingViewerPawn::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookVector.X * 0.2f);
	AddControllerPitchInput(LookVector.Y * 0.2f);
}

void AFlyingViewerPawn::Interaction()
{
	if (ABattleRoyalPlayerController* BattleController = Cast<ABattleRoyalPlayerController>(GetController()))
		BattleController->Server_RequestDive();
}
