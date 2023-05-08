#include "BattleRoyalCharacter.h"

#include "InventoryComponent.h"
#include "StatComponent.h"
#include "WeaponComponent.h"
#include "BattleRoyalPlayerController.h"
#include "HUDWidget.h"
#include "NoticeWidget.h"
#include "InventoryPreview.h"
#include "Door.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

ABattleRoyalCharacter::ABattleRoyalCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;

	TargetReticleSpreadFactor = DefaultSpreadFactor;
	CurrReticleSpreadFactor = TargetReticleSpreadFactor;
	TargetSpringArmZPos = DefaultSpringArmZPos;
	
	// Input
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_BattleRoyal(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/BattleRoyal/Input/IMC_BattleRoyal.IMC_BattleRoyal'"));
	if (IMC_BattleRoyal.Succeeded())
		BattleRoyalMappingContext = IMC_BattleRoyal.Object;
	
	// Collision
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);
	GetCapsuleComponent()->SetCapsuleRadius(40.f);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABattleRoyalCharacter::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ABattleRoyalCharacter::OnEndOverlap);

	// Movement
	GetCharacterMovement()->MaxWalkSpeedCrouched = DefaultCrouchSpeed;
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	GetCharacterMovement()->MaxSwimSpeed = DefaultSwimSpeed;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(DefaultCrouchHalfHeight);

	// Mesh
	ParachuteMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Parachute"));
	ParachuteMeshComponent->SetupAttachment(GetMesh());
	ParachuteMeshComponent->SetRelativeLocation(FVector(0.6f, 0.f, 177.1f));
	ParachuteMeshComponent->SetRelativeRotation(FRotator(0.0f, 90.f, 0.f));
	ParachuteMeshComponent->SetRelativeScale3D(FVector(0.f));
	ParachuteMeshComponent->SetCollisionProfileName("NoCollision");
	ParachuteMeshComponent->SetVisibility(false, true);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Parachute(TEXT("/Script/Engine.StaticMesh'/Game/BattleRoyal/Characters/Parachute/SM_Parachute.SM_Parachute'"));
	if (SM_Parachute.Succeeded())
		ParachuteMeshComponent->SetStaticMesh(SM_Parachute.Object);
	
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -97.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Military_Head1(TEXT("/Script/Engine.SkeletalMesh'/Game/BattleRoyal/Characters/Soldier/Mesh/Body/SK_Military_Head1.SK_Military_Head1'"));
	if (SK_Military_Head1.Succeeded())
	{
		DefaultHeadMesh = SK_Military_Head1.Object;
		GetMesh()->SetSkeletalMesh(DefaultHeadMesh);
	}

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	WeaponMeshComponent->SetupAttachment(GetMesh());
	
	ShirtMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shirt"));
	ShirtMeshComponent->SetupAttachment(GetMesh());
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Torso_Male(TEXT("/Script/Engine.SkeletalMesh'/Game/BattleRoyal/Characters/Soldier/Mesh/Rare/Torso_Male.Torso_Male'"));
	if (Torso_Male.Succeeded())
	{
		DefaultShirtMesh = Torso_Male.Object;
		ShirtMeshComponent->SetSkeletalMesh(DefaultShirtMesh);
	}

	HandsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands"));
	HandsMeshComponent->SetupAttachment(GetMesh());
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Military_Hands_Male(TEXT("/Script/Engine.SkeletalMesh'/Game/BattleRoyal/Characters/Soldier/Mesh/Body/SK_Military_Hands_Male.SK_Military_Hands_Male'"));
	if (SK_Military_Hands_Male.Succeeded())
	{
		DefaultHandsMesh = SK_Military_Hands_Male.Object;
		HandsMeshComponent->SetSkeletalMesh(DefaultHandsMesh);
	}
	
	PantsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Pants"));
	PantsMeshComponent->SetupAttachment(GetMesh());
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Legs_Male(TEXT("/Script/Engine.SkeletalMesh'/Game/BattleRoyal/Characters/Soldier/Mesh/Rare/Legs_Male.Legs_Male'"));
	if (Legs_Male.Succeeded())
	{
		DefaultPantsMesh = Legs_Male.Object;
		PantsMeshComponent->SetSkeletalMesh(DefaultPantsMesh);
	}
	
	BootsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Boots"));
	BootsMeshComponent->SetupAttachment(GetMesh());
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Military_Foot_Male(TEXT("/Script/Engine.SkeletalMesh'/Game/BattleRoyal/Characters/Soldier/Mesh/Body/SK_Military_Foot_Male.SK_Military_Foot_Male'"));
	if (SK_Military_Foot_Male.Succeeded())
	{
		DefaultBootsMesh = SK_Military_Foot_Male.Object;
		BootsMeshComponent->SetSkeletalMesh(DefaultBootsMesh);
	}
	
	VestMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Vest"));
	VestMeshComponent->SetupAttachment(GetMesh());

	BackpackMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Backpack"));
	BackpackMeshComponent->SetupAttachment(GetMesh());
	
	BackpackStringMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackpackString"));
	BackpackStringMeshComponent->SetupAttachment(GetMesh());

	HelmetMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helmet"));
	HelmetMeshComponent->SetupAttachment(GetMesh());

	HelmetOutfitMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetOutfit"));
	HelmetOutfitMeshComponent->SetupAttachment(GetMesh());

	// Animation
	static ConstructorHelpers::FClassFinder<UAnimInstance> ABP_BattleRoyal(TEXT("/Script/Engine.AnimBlueprint'/Game/BattleRoyal/Characters/Animations/ABP_BattleRoyal.ABP_BattleRoyal_C'"));
	if (ABP_BattleRoyal.Succeeded())
		AnimBPClass = ABP_BattleRoyal.Class;

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimClass(AnimBPClass);
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	
	ShirtMeshComponent->SetLeaderPoseComponent(GetMesh());
	PantsMeshComponent->SetLeaderPoseComponent(GetMesh());
	BootsMeshComponent->SetLeaderPoseComponent(GetMesh());
	HandsMeshComponent->SetLeaderPoseComponent(GetMesh());
	VestMeshComponent->SetLeaderPoseComponent(GetMesh());
	BackpackMeshComponent->SetLeaderPoseComponent(GetMesh());
	BackpackStringMeshComponent->SetLeaderPoseComponent(GetMesh());
	HelmetMeshComponent->SetLeaderPoseComponent(GetMesh());
	HelmetOutfitMeshComponent->SetLeaderPoseComponent(GetMesh());
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Drinking_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Characters/Animations/Actions/Drinking_Montage.Drinking_Montage'"));
	if (Drinking_Montage.Succeeded())
		DrinkingMontage = Drinking_Montage.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Diving_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Characters/Animations/Actions/Diving_Montage.Diving_Montage'"));
	if (Diving_Montage.Succeeded())
		DivingMontage = Diving_Montage.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Gliding_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Characters/Animations/Actions/Gliding_Montage.Gliding_Montage'"));
	if (Gliding_Montage.Succeeded())
		GlidingMontage = Gliding_Montage.Object;
	
	// Spring Arm & Camera
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(GetMesh());
	SpringArmComponent->SetRelativeLocation(FVector(-60.f, 0.f, DefaultSpringArmZPos));
	SpringArmComponent->bUsePawnControlRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	// Custom Component
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("Firing"));

	// Timeline
	SmoothADSTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineADS"));
	SmoothADSInterpFunction.BindUFunction(this, TEXT("SmoothADSInterp"));

	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveFloat_SmoothADS(TEXT("/Script/Engine.CurveFloat'/Game/BattleRoyal/Characters/Animations/CurveFloat_SmoothADS.CurveFloat_SmoothADS'"));
	if (CurveFloat_SmoothADS.Succeeded())
		SmoothCurveADS = CurveFloat_SmoothADS.Object;

	SmoothParachuteTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineParachute"));
	SmoothParachuteInterpFunction.BindUFunction(this, TEXT("SmoothParachuteInterp"));

	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveFloat_SmoothParachute(TEXT("/Script/Engine.CurveFloat'/Game/BattleRoyal/Characters/Animations/CurveFloat_SmoothParachute.CurveFloat_SmoothParachute'"));
	if (CurveFloat_SmoothParachute.Succeeded())
		SmoothCurveParachute = CurveFloat_SmoothParachute.Object;

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundWave> SW_Parachute_Opening(TEXT("/Script/Engine.SoundWave'/Game/BattleRoyal/Audio/SW_Parachute_Opening.SW_Parachute_Opening'"));
	if (SW_Parachute_Opening.Succeeded())
		ParachuteOpeningSound = SW_Parachute_Opening.Object;

	static ConstructorHelpers::FObjectFinder<USoundWave> SW_Parachute_Wind(TEXT("/Script/Engine.SoundWave'/Game/BattleRoyal/Audio/SW_Parachute_Wind.SW_Parachute_Wind'"));
	if (SW_Parachute_Wind.Succeeded())
		ParachuteWindSound = SW_Parachute_Wind.Object;
}

void ABattleRoyalCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ControllerOwner->GetLocalPlayer()))
			{
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(BattleRoyalMappingContext, 0);
			}
		}
	}

	SmoothADSTimeline->AddInterpFloat(SmoothCurveADS, SmoothADSInterpFunction);
	SmoothADSTimeline->SetLooping(false);

	SmoothParachuteTimeline->AddInterpFloat(SmoothCurveParachute, SmoothParachuteInterpFunction);
	SmoothParachuteTimeline->SetLooping(false);
}

void ABattleRoyalCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	CurrMeshPitchOffset = FMath::FInterpTo(CurrMeshPitchOffset, TargetMeshPitchOffset, DeltaSeconds, 2.f);
	CurrMeshRollOffset = FMath::FInterpTo(CurrMeshRollOffset, TargetMeshRollOffset, DeltaSeconds, 2.f);
	GetMesh()->SetRelativeRotation(FRotator(CurrMeshPitchOffset, -90.f, CurrMeshRollOffset));
	
	if (GetLocalRole() == ROLE_Authority || GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (GetCharacterMovement()->IsCrouching())
			TargetSpringArmZPos = (MoveForwardValue || MoveRightValue) ? CrouchMoveSpringArmZPos : CrouchIdleSpringArmZPos;
		else
			TargetSpringArmZPos = DefaultSpringArmZPos;
	
		FVector Location = SpringArmComponent->GetRelativeLocation();
		Location.Z = FMath::FInterpTo(Location.Z, TargetSpringArmZPos, DeltaSeconds, 6.f);
		SpringArmComponent->SetRelativeLocation(Location);

		if (FallingState == EFallingState::Glide)
			TargetSpringArmLength = 1500.f;
		else
			TargetSpringArmLength = 300.f;

		float ArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, TargetSpringArmLength, DeltaSeconds, 4.f);
		SpringArmComponent->TargetArmLength = ArmLength;
	}
	
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (FallingState == EFallingState::Diving)
		{
			FFindFloorResult Result;
			GetCharacterMovement()->ComputeFloorDist(GetCapsuleComponent()->GetComponentLocation(),
				60000.f, 60000.f, Result, 50.f);

			if (Result.bBlockingHit && Result.FloorDist < 30000.f)
			{
				Server_RequestGlide();
				FallingState = EFallingState::Glide;
			}
		}
		else if (FallingState == EFallingState::Glide)
		{
			FFindFloorResult Result;
			GetCharacterMovement()->ComputeFloorDist(GetCapsuleComponent()->GetComponentLocation(),
				30000.f, 30000.f, Result, 50.f);

			if (Result.bBlockingHit && Result.FloorDist < 500.f)
			{
				Server_RequestLand();
				FallingState = EFallingState::Land;
			}
		}
		
		CurrReticleSpreadFactor = FMath::FInterpTo(CurrReticleSpreadFactor, TargetReticleSpreadFactor, DeltaSeconds, 10.f);

		if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
		{
			UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
			if (IsValid(HUDWidget))
				HUDWidget->RefreshReticleUI(CurrReticleSpreadFactor);
		}
	}
	else
	{
		CurrAimPitch = FMath::FInterpTo(CurrAimPitch, TargetAimPitch, DeltaSeconds, 10.f);
	}
}

void ABattleRoyalCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleRoyalCharacter, MoveForwardValue);
	DOREPLIFETIME(ABattleRoyalCharacter, MoveRightValue);
	DOREPLIFETIME(ABattleRoyalCharacter, TargetAimPitch);
	DOREPLIFETIME(ABattleRoyalCharacter, TargetMeshRollOffset);
	DOREPLIFETIME(ABattleRoyalCharacter, TargetMeshPitchOffset);
}

float ABattleRoyalCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HasAuthority() == false)
		return DamageAmount;
	
	StatComponent->TakeDamage(DamageAmount);
	return DamageAmount;
}

void ABattleRoyalCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetLocalRole() == ROLE_SimulatedProxy)
		return;
	
	if (OtherActor->GetClass() == ADoor::StaticClass())
	{
		OverlappedDoor = Cast<ADoor>(OtherActor);
		
		if (GetLocalRole() == ROLE_AutonomousProxy && InventoryComponent->CanInteraction())
		{
			if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
			{
				UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
				if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
					HUDWidget->NoticeWidget->ShowInteractionText(TEXT("문 여닫기"));
			}
		}
	}
}

void ABattleRoyalCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetLocalRole() == ROLE_SimulatedProxy)
		return;
	
	if (OtherActor->GetClass() == ADoor::StaticClass())
	{
		OverlappedDoor = nullptr;

		if (GetLocalRole() == ROLE_AutonomousProxy && InventoryComponent->CanInteraction())
		{
			if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
			{
				UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
				if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
					HUDWidget->NoticeWidget->HideInteractionText();
			}
		}
	}
}

void ABattleRoyalCharacter::MoveForward(const FInputActionValue& Value)
{
	MoveForwardValue = Value.Get<float>();
	AddMovementInput(GetActorForwardVector(), MoveForwardValue);
	
	if (PrevMoveForwardValue != MoveForwardValue)
	{
		Server_MoveForwardValue(MoveForwardValue);
		PrevMoveForwardValue = MoveForwardValue;
	}
}

bool ABattleRoyalCharacter::Server_MoveForwardValue_Validate(float NewMoveForwardValue)
{
	return -1.f <= NewMoveForwardValue && NewMoveForwardValue <= 1.f;
}

void ABattleRoyalCharacter::Server_MoveForwardValue_Implementation(float NewMoveForwardValue)
{
	MoveForwardValue = NewMoveForwardValue;
	TargetMeshRollOffset = (FallingState == EFallingState::Land) ? 0.f : MoveForwardValue * 10.f;
}

void ABattleRoyalCharacter::MoveRight(const FInputActionValue& Value)
{
	MoveRightValue = Value.Get<float>();
	AddMovementInput(GetActorRightVector(), MoveRightValue);
	
	if (PrevMoveRightValue != MoveRightValue)
	{
		Server_MoveRightValue(MoveRightValue);
		PrevMoveRightValue = MoveRightValue;
	}
}

bool ABattleRoyalCharacter::Server_MoveRightValue_Validate(float NewMoveRightValue)
{
	return -1.f <= NewMoveRightValue && NewMoveRightValue <= 1.f;
}

void ABattleRoyalCharacter::Server_MoveRightValue_Implementation(float NewMoveRightValue)
{
	MoveRightValue = NewMoveRightValue;
	TargetMeshPitchOffset = (FallingState == EFallingState::Land) ? 0.f : MoveRightValue * 10.f;
}

void ABattleRoyalCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookVector.X * 0.2f);
	AddControllerPitchInput(LookVector.Y * 0.2f);

	CurrAimPitch = (GetControlRotation() - GetActorRotation()).GetNormalized().Pitch;
	if (FMath::Abs(CurrAimPitch - PrevAimPitch) > 5.f)
	{
		Server_AimPitch(CurrAimPitch);
		PrevAimPitch = CurrAimPitch;
	}
}

bool ABattleRoyalCharacter::Server_AimPitch_Validate(float NewAimPitch)
{
	return -90.f <= NewAimPitch && NewAimPitch <= 90.f;
}

void ABattleRoyalCharacter::Server_AimPitch_Implementation(float NewAimPitch)
{
	TargetAimPitch = NewAimPitch;
}

void ABattleRoyalCharacter::ToggleCrouch()
{
	if (GetCharacterMovement()->IsCrouching())
		UnCrouch(true);
	else if (CanCrouching())
		Crouch(true);
}

void ABattleRoyalCharacter::StartJump()
{
	if (CanJump())
		Jump();
}

void ABattleRoyalCharacter::StopJump()
{
	StopJumping();
}

void ABattleRoyalCharacter::Sprint(const FInputActionValue& Value)
{
	bool bPressed = Value.Get<bool>();
	if (bPressed && CanSprint() == false)
		return;
	
	Server_Sprint(bPressed);
}

void ABattleRoyalCharacter::Server_Sprint_Implementation(bool bWantSprint)
{
	if (bWantSprint && CanSprint() == false)
		return;

	Multicast_Sprint(bWantSprint);
}

void ABattleRoyalCharacter::Multicast_Sprint_Implementation(bool bWantSprint)
{
	bIsSprinting = bWantSprint;
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? DefaultSprintSpeed : DefaultWalkSpeed;
}

void ABattleRoyalCharacter::ADS(const FInputActionValue& Value)
{
	bool bPressed = Value.Get<bool>();
	if (bPressed && CanADS() == false)
		return;
	
	Server_ADS(bPressed);
}

void ABattleRoyalCharacter::Server_ADS_Implementation(bool bWantADS)
{
	if (bWantADS)
	{
		if (CanADS() == false)
			return;
		
		if (InventoryComponent->GetUsingWeaponType() == EWeaponType::None)
			return;
		
		bIsADSing = true;
		TargetReticleSpreadFactor = ADSSpreadFactor;
		Client_ADS(bIsADSing);
	}
	else
	{
		bIsADSing = false;
		TargetReticleSpreadFactor = DefaultSpreadFactor;
		Client_ADS(bIsADSing);
	}
}

void ABattleRoyalCharacter::Client_ADS_Implementation(bool bWantADS)
{
	if (bWantADS)
	{
		bIsADSing = true;
		TargetReticleSpreadFactor = ADSSpreadFactor;
		SmoothADSTimeline->Play();
	}
	else
	{
		bIsADSing = false;
		TargetReticleSpreadFactor = DefaultSpreadFactor;
		SmoothADSTimeline->Reverse();
	}
}

void ABattleRoyalCharacter::SmoothADSInterp(float Value)
{
	CameraComponent->SetFieldOfView(FMath::Lerp(90.f, 37.f, Value));
}

void ABattleRoyalCharacter::Fire(const FInputActionValue& Value)
{
	bool bPressed = Value.Get<bool>();
	bPressed ? WeaponComponent->StartFire() : WeaponComponent->StopFire();
}

void ABattleRoyalCharacter::Reload()
{
	WeaponComponent->RequestReload();
}

void ABattleRoyalCharacter::SelectPrimary()
{
	InventoryComponent->RequestSelectWeaponSlot(EWeaponSlotType::Primary);
}

void ABattleRoyalCharacter::SelectSecondary()
{
	InventoryComponent->RequestSelectWeaponSlot(EWeaponSlotType::Secondary);
}

void ABattleRoyalCharacter::SelectTertiary()
{
	InventoryComponent->RequestSelectWeaponSlot(EWeaponSlotType::Tertiary);
}

void ABattleRoyalCharacter::Interaction()
{
	if (FallingState == EFallingState::Land)
		bIsHealCasting ? StatComponent->Server_CancelHeal() : InventoryComponent->Interaction();
	else if (FallingState == EFallingState::Diving)
		Server_RequestGlide();
}

void ABattleRoyalCharacter::UseHealthKit()
{
	if (CanUseHealthKit() == false)
		return;
	
	InventoryComponent->Server_UseHealthKit();
}

void ABattleRoyalCharacter::Multicast_SetFallingState_Implementation(EFallingState NewFallingState)
{
	FallingState = NewFallingState;
	
	if (FallingState == EFallingState::Diving)
	{
		PlayAnimMontage(DivingMontage);
		GetCharacterMovement()->AirControl = 0.7f;
		GetCharacterMovement()->MaxWalkSpeed = DefaultDiveSpeed;
		GetCharacterMovement()->GravityScale = 0.5f;

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			WindAudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, ParachuteWindSound, GetActorLocation());
			if (IsValid(WindAudioComponent))
				WindAudioComponent->SetVolumeMultiplier(0.6f);
			
			if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
			{
				UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
				if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
					HUDWidget->NoticeWidget->ShowInteractionText(TEXT("낙하산 펼치기"));
			}
		}
	}
	else if (FallingState == EFallingState::Glide)
	{
		PlayAnimMontage(GlidingMontage);
		ParachuteMeshComponent->SetVisibility(true, true);
		SmoothParachuteTimeline->PlayFromStart();
		
		GetCharacterMovement()->AirControl = 1.f;
		GetCharacterMovement()->MaxWalkSpeed = DefaultGlideSpeed;
		GetCharacterMovement()->GravityScale = 0.2f;
		GetCharacterMovement()->Velocity *= 0.25f;

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			if (IsValid(WindAudioComponent))
				WindAudioComponent->SetVolumeMultiplier(0.2f);
			
			UGameplayStatics::PlaySoundAtLocation(this, ParachuteOpeningSound, GetActorLocation());
			
			if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
			{
				UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
				if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
					HUDWidget->NoticeWidget->HideInteractionText();
			}
		}
	}
	else if (FallingState == EFallingState::Land)
	{
		StopAnimMontage();
		ParachuteMeshComponent->SetVisibility(false, true);
		GetCharacterMovement()->AirControl = 0.05f;
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
		GetCharacterMovement()->GravityScale = 1.f;

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			if (IsValid(WindAudioComponent))
				WindAudioComponent->Stop();
		}
	}
}

void ABattleRoyalCharacter::Server_RequestGlide_Implementation()
{
	if (FallingState == EFallingState::Diving)
		Multicast_SetFallingState(EFallingState::Glide);
}

void ABattleRoyalCharacter::Server_RequestLand_Implementation()
{
	if (FallingState == EFallingState::Glide)
	{
		TargetMeshPitchOffset = 0.f;
		TargetMeshRollOffset = 0.f;
		Multicast_SetFallingState(EFallingState::Land);
	}
}

void ABattleRoyalCharacter::SmoothParachuteInterp(float Value)
{
	ParachuteMeshComponent->SetRelativeScale3D(FVector(FMath::Lerp(0.f, 0.25f, Value)));
}

bool ABattleRoyalCharacter::CanCrouching()
{
	return !(bIsSprinting || bIsDead || GetCharacterMovement()->IsCrouching() ||
		GetCharacterMovement()->IsSwimming() || GetCharacterMovement()->IsFalling());
}

bool ABattleRoyalCharacter::CanJump()
{
	return !(bIsADSing || bIsFiring || bIsDead || bIsHealCasting ||
		GetCharacterMovement()->IsSwimming() || GetCharacterMovement()->IsFalling());
}

bool ABattleRoyalCharacter::CanSprint()
{
	return !(bIsSprinting || bIsADSing || bIsFiring || bIsReloading || bIsDead || bIsHealCasting ||
		GetCharacterMovement()->IsSwimming() || GetMovementComponent()->IsFalling());
}

bool ABattleRoyalCharacter::CanADS()
{
	return !(bIsSprinting || bIsADSing || bIsDead || bIsHealCasting ||
		GetCharacterMovement()->IsSwimming() || GetMovementComponent()->IsFalling());
}

bool ABattleRoyalCharacter::CanUseHealthKit()
{
	if (bIsSprinting || bIsADSing || bIsFiring || bIsReloading || bIsDead || bIsHealCasting ||
		GetCharacterMovement()->IsSwimming() || GetCharacterMovement()->IsFalling())
		return false;
			
	if (StatComponent->GetCurrHP() >= StatComponent->GetMaxHP())
	{
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
			{
				UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
				if (IsValid(HUDWidget) && IsValid(HUDWidget->NoticeWidget))
					ControllerOwner->GetHUDWidget()->NoticeWidget->ShowNoticeText(TEXT("이미 체력이 가득 차 있습니다"));
			}
		}
		return false;
	}

	return true;
}

bool ABattleRoyalCharacter::CanOpenUI()
{
	return !(bIsSprinting || bIsADSing || bIsFiring || bIsDead);
}

void ABattleRoyalCharacter::SetWeaponMesh(EWeaponType WeaponType, USkeletalMesh* NewMesh)
{
	WeaponMeshComponent->SetSkeletalMesh(NewMesh);
	InventoryComponent->SetUsingWeaponType(WeaponType);
	
	FAttachmentTransformRules Rules { EAttachmentRule::SnapToTarget, true };
	
	switch (WeaponType)
	{
	case EWeaponType::None:
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
			{
				UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
				if (IsValid(HUDWidget))
					HUDWidget->ShowUnarmedReticle();
			}
		}
		break;
		
	case EWeaponType::Pistol:
		WeaponMeshComponent->AttachToComponent(GetMesh(), Rules, TEXT("pistol_socket"));
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
			{
				UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
				if (IsValid(HUDWidget))
					HUDWidget->ShowPistolReticle();
			}
		}
		break;
		
	case EWeaponType::Rifle:
		WeaponMeshComponent->AttachToComponent(GetMesh(), Rules, TEXT("rifle_socket"));
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
			{
				UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
				if (IsValid(HUDWidget))
					HUDWidget->ShowRifleReticle();
			}
		}
		break;
		
	case EWeaponType::Shotgun:
		WeaponMeshComponent->AttachToComponent(GetMesh(), Rules, TEXT("shotgun_socket"));
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
			{
				UHUDWidget* HUDWidget = ControllerOwner->GetHUDWidget();
				if (IsValid(HUDWidget))
					HUDWidget->ShowShotgunReticle();
			}
		}
		break;
	}

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
			ControllerOwner->GetInventoryPreview()->SetWeaponMesh(WeaponType, NewMesh);
	}
}

void ABattleRoyalCharacter::SetArmorMesh(EArmorType ArmorType, USkeletalMesh* NewMesh, USkeletalMesh* NewAttachmentMesh)
{
	switch (ArmorType)
	{
	case EArmorType::Helmet:
		HelmetMeshComponent->SetSkeletalMesh(NewMesh);
		HelmetOutfitMeshComponent->SetSkeletalMesh(NewAttachmentMesh);
		break;
		
	case EArmorType::Shirt:
		if (NewMesh)
			ShirtMeshComponent->SetSkeletalMesh(NewMesh);
		else
			ShirtMeshComponent->SetSkeletalMesh(DefaultShirtMesh);
		break;
		
	case EArmorType::Vest:
		VestMeshComponent->SetSkeletalMesh(NewMesh);
		break;
		
	case EArmorType::Backpack:
		BackpackMeshComponent->SetSkeletalMesh(NewMesh);
		BackpackStringMeshComponent->SetSkeletalMesh(NewAttachmentMesh);
		break;
		
	case EArmorType::Pants:
		if (NewMesh)
			PantsMeshComponent->SetSkeletalMesh(NewMesh);
		else
			PantsMeshComponent->SetSkeletalMesh(DefaultPantsMesh);
		break;
		
	case EArmorType::Boots:
		if (NewMesh)
			BootsMeshComponent->SetSkeletalMesh(NewMesh);
		else
			BootsMeshComponent->SetSkeletalMesh(DefaultBootsMesh);
		break;
	}

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (ABattleRoyalPlayerController* ControllerOwner = Cast<ABattleRoyalPlayerController>(GetController()))
			ControllerOwner->GetInventoryPreview()->SetArmorMesh(ArmorType, NewMesh, NewAttachmentMesh);
	}
}
