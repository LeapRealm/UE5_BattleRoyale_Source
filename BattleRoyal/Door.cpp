#include "Door.h"

#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PlusAxisBox"));
	OverlapBox->SetupAttachment(RootComponent);
	OverlapBox->SetRelativeLocation(FVector(-47.f, 0.f, 103.f));
	OverlapBox->SetBoxExtent(FVector(74.f, 200.f, 100.f));
	OverlapBox->SetLineThickness(2.f);
	OverlapBox->ShapeColor = FColor::Red;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

	// Timeline
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveFloat_SmoothDoor(TEXT("/Script/Engine.CurveFloat'/Game/BattleRoyal/Characters/Animations/CurveFloat_SmoothDoor.CurveFloat_SmoothDoor'"));
	if (CurveFloat_SmoothDoor.Succeeded())
		SmoothCurveDoor = CurveFloat_SmoothDoor.Object;
	
	OpenPlusYAxisTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PlusYAxisTimeline"));
	OpenPlusYAxisInterpFunction.BindUFunction(this, TEXT("OpenPlusYAxisInterp"));

	OpenMinusYAxisTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MinusYAxisTimeline"));
	OpenMinusYAxisInterpFunction.BindUFunction(this, TEXT("OpenMinusYAxisInterp"));
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	OpenPlusYAxisTimeline->AddInterpFloat(SmoothCurveDoor, OpenPlusYAxisInterpFunction);
	OpenPlusYAxisTimeline->SetLooping(false);

	OpenMinusYAxisTimeline->AddInterpFloat(SmoothCurveDoor, OpenMinusYAxisInterpFunction);
	OpenMinusYAxisTimeline->SetLooping(false);
}

void ADoor::RotateDoor(const FVector& PlayerPos)
{
	if (HasAuthority() == false)
		return;
	
	if (OpenPlusYAxisTimeline->IsPlaying() || OpenMinusYAxisTimeline->IsPlaying())
		return;
	
	FVector DoorDir = MeshComponent->GetRightVector();
	FVector DoorToPlayerDir = (PlayerPos - MeshComponent->GetComponentLocation()).GetSafeNormal();
	Multicast_RotateDoor(DoorDir.Dot(DoorToPlayerDir) >= 0);
}

void ADoor::OpenPlusYAxisInterp(float Value)
{
	FRotator NewRotation = FRotator(0.f, FMath::Lerp(0.f, -90.f, Value), 0.f);
	MeshComponent->SetRelativeRotation(NewRotation);
}

void ADoor::OpenMinusYAxisInterp(float Value)
{
	FRotator NewRotation = FRotator(0.f, FMath::Lerp(0.f, 90.f, Value), 0.f);
	MeshComponent->SetRelativeRotation(NewRotation);
}

void ADoor::Multicast_RotateDoor_Implementation(bool bIsPlayerPlusY)
{
	switch (State)
	{
	case EDoorState::Closed:
		if (bIsPlayerPlusY)
		{
			OpenMinusYAxisTimeline->Play();
			State = EDoorState::OpenedMinusYAxis;
		}
		else
		{
			OpenPlusYAxisTimeline->Play();
			State = EDoorState::OpenedPlusYAxis;
		}
		break;
	case EDoorState::OpenedPlusYAxis:
		OpenPlusYAxisTimeline->Reverse();
		State = EDoorState::Closed;
		break;
	case EDoorState::OpenedMinusYAxis:
		OpenMinusYAxisTimeline->Reverse();
		State = EDoorState::Closed;
		break;
	}
}
