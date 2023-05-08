#include "ElectricField.h"

#include "BattleRoyalGameState.h"

#include "Kismet/GameplayStatics.h"

AElectricField::AElectricField()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	AActor::SetReplicateMovement(true);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetIsReplicated(true);
	RootComponent = MeshComponent;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_ElectricCylinder(TEXT("/Script/Engine.StaticMesh'/Game/BattleRoyal/ElectricField/Meshes/SM_ElectricCylinder.SM_ElectricCylinder'"));
	if (SM_ElectricCylinder.Succeeded())
		MeshComponent->SetStaticMesh(SM_ElectricCylinder.Object);
}

void AElectricField::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority() == false)
		return;
	
	if (ABattleRoyalGameState* GameState = Cast<ABattleRoyalGameState>(UGameplayStatics::GetGameState(this)))
	{
		MeshComponent->SetVisibility(GameState->bVisibleCurrCircle, true);
		SetActorLocation(GameState->CurrCirclePos);

		float Scale = GameState->CurrCircleRadius / 100.f * 2.f;
		SetActorScale3D(FVector(Scale, Scale, 2000.f));
	}
}
