#include "Aircraft.h"

#include "BattleRoyalGameMode.h"

#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AAircraft::AAircraft()
{
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;
	
	GetCapsuleComponent()->SetCollisionProfileName("NoCollision");

	GetCharacterMovement()->MaxFlySpeed = 12500.f;
	GetCharacterMovement()->DefaultLandMovementMode = MOVE_Flying;
	GetCharacterMovement()->DefaultWaterMovementMode = MOVE_Flying;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->NetworkSmoothingMode = ENetworkSmoothingMode::Disabled;
	
	GetMesh()->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	GetMesh()->SetCollisionProfileName("NoCollision");
	
	// Body
	BodyComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	BodyComponent->SetupAttachment(GetMesh());
	BodyComponent->SetRelativeLocation(FVector(-182.f, -8.1f, -257.7f));
	BodyComponent->SetCollisionProfileName("NoCollision");
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Aircraft_Body(TEXT("/Script/Engine.StaticMesh'/Game/BattleRoyalMap/Aircraft/Meshes/SM_Aircraft_Body.SM_Aircraft_Body'"));
	if (SM_Aircraft_Body.Succeeded())
		BodyComponent->SetStaticMesh(SM_Aircraft_Body.Object);

	// RearRampDown
	RearRampDownComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RearRampDown"));
	RearRampDownComponent->SetupAttachment(GetMesh());
	RearRampDownComponent->SetRelativeLocation(FVector(491.f, 3.8f, -184.7f));
	RearRampDownComponent->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	RearRampDownComponent->SetCollisionProfileName("NoCollision");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Aircraft_RearRamp_Down(TEXT("/Script/Engine.StaticMesh'/Game/BattleRoyalMap/Aircraft/Meshes/SM_Aircraft_RearRamp_Down.SM_Aircraft_RearRamp_Down'"));
	if (SM_Aircraft_RearRamp_Down.Succeeded())
		RearRampDownComponent->SetStaticMesh(SM_Aircraft_RearRamp_Down.Object);

	// RearRampUp
	RearRampUpComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RearRampUp"));
	RearRampUpComponent->SetupAttachment(GetMesh());
	RearRampUpComponent->SetRelativeLocation(FVector(1233.f, 9.8f, 69.2f));
	RearRampUpComponent->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	RearRampUpComponent->SetCollisionProfileName("NoCollision");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Aircraft_RearRamp_Up(TEXT("/Script/Engine.StaticMesh'/Game/BattleRoyalMap/Aircraft/Meshes/SM_Aircraft_RearRamp_Up.SM_Aircraft_RearRamp_Up'"));
	if (SM_Aircraft_RearRamp_Up.Succeeded())
		RearRampUpComponent->SetStaticMesh(SM_Aircraft_RearRamp_Up.Object);

	// Propeller
	PropellerFirstComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PropellerFirst"));
	PropellerFirstComponent->SetupAttachment(GetMesh());
	PropellerFirstComponent->SetRelativeLocation(FVector(-391.f, 1036.8f, 99.2f));
	PropellerFirstComponent->SetCollisionProfileName("NoCollision");

	PropellerSecondComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PropellerSecond"));
	PropellerSecondComponent->SetupAttachment(GetMesh());
	PropellerSecondComponent->SetRelativeLocation(FVector(-391.f, 511.8f, 87.2f));
	PropellerSecondComponent->SetCollisionProfileName("NoCollision");

	PropellerThirdComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PropellerThird"));
	PropellerThirdComponent->SetupAttachment(GetMesh());
	PropellerThirdComponent->SetRelativeLocation(FVector(-384.f, -514.1f, 87.2f));
	PropellerThirdComponent->SetCollisionProfileName("NoCollision");

	PropellerFourthComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PropellerFourth"));
	PropellerFourthComponent->SetupAttachment(GetMesh());
	PropellerFourthComponent->SetRelativeLocation(FVector(-376.f, -1040.1f, 99.2f));
	PropellerFourthComponent->SetCollisionProfileName("NoCollision");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Aircraft_Propeller(TEXT("/Script/Engine.StaticMesh'/Game/BattleRoyalMap/Aircraft/Meshes/SM_Aircraft_Propeller.SM_Aircraft_Propeller'"));
	if (SM_Aircraft_Propeller.Succeeded())
	{
		PropellerFirstComponent->SetStaticMesh(SM_Aircraft_Propeller.Object);
		PropellerSecondComponent->SetStaticMesh(SM_Aircraft_Propeller.Object);
		PropellerThirdComponent->SetStaticMesh(SM_Aircraft_Propeller.Object);
		PropellerFourthComponent->SetStaticMesh(SM_Aircraft_Propeller.Object);
	}

	// Sound
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bOverrideAttenuation = true;

	FSoundAttenuationSettings Settings;
	Settings.AttenuationShapeExtents.X = 25000.f;
	Settings.FalloffDistance = 25000.f;
	AudioComponent->AdjustAttenuation(Settings);
	
	static ConstructorHelpers::FObjectFinder<USoundWave> SW_AircraftSound(TEXT("/Script/Engine.SoundWave'/Game/BattleRoyal/Audio/SW_AircraftSound.SW_AircraftSound'"));
	if (SW_AircraftSound.Succeeded())
		AudioComponent->SetSound(SW_AircraftSound.Object);
}

void AAircraft::BeginPlay()
{
	Super::BeginPlay();
	
	SetActorLocation(StartLocation);
	SetActorRotation(FRotator(0.f, UKismetMathLibrary::FindLookAtRotation(StartLocation, AllDiveLocation).Yaw, 0.f));
	StartToAllDiveDist = (StartLocation - AllDiveLocation).Length();
}

void AAircraft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority)
	{
		if (State == EAircraftState::StandBy)
			return;

		GetCharacterMovement()->AddInputVector(GetActorForwardVector(), true);
		
		if (State == EAircraftState::Going)
		{
			float Distance = (StartLocation - GetActorLocation()).Length();
			if (Distance >= StartToAllDiveDist)
			{
				if (ABattleRoyalGameMode* BattleRoyalGameMode = Cast<ABattleRoyalGameMode>(UGameplayStatics::GetGameMode(this)))
					BattleRoyalGameMode->AllDive();
				
				State = EAircraftState::AllDived;
			}
		}
		else if (State == EAircraftState::AllDived)
		{
			float Distance = (AllDiveLocation - GetActorLocation()).Length();
			if (Distance >= AllDiveToEndDist)
				Destroy();
		}
	}
	else
	{
		FRotator Delta = FRotator(0.f, 0.f, -720.f * DeltaTime);
		PropellerFirstComponent->AddLocalRotation(Delta);
		PropellerSecondComponent->AddLocalRotation(Delta);
		PropellerThirdComponent->AddLocalRotation(Delta);
		PropellerFourthComponent->AddLocalRotation(Delta);
	}
}
