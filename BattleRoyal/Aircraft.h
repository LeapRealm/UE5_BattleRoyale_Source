#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Aircraft.generated.h"

UENUM()
enum class EAircraftState : uint8
{
	StandBy,
	Going,
	AllDived,
};

UCLASS()
class BATTLEROYAL_API AAircraft : public ACharacter
{
	GENERATED_BODY()

public:
	AAircraft();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	void SetStarted() { State = EAircraftState::Going; }

private:
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<UStaticMeshComponent> BodyComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<UStaticMeshComponent> RearRampDownComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<UStaticMeshComponent> RearRampUpComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<UStaticMeshComponent> PropellerFirstComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<UStaticMeshComponent> PropellerSecondComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<UStaticMeshComponent> PropellerThirdComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<UStaticMeshComponent> PropellerFourthComponent;

	UPROPERTY(VisibleAnywhere, Category="Audio")
	TObjectPtr<UAudioComponent> AudioComponent;

public:
	UPROPERTY(VisibleAnywhere)
	EAircraftState State = EAircraftState::StandBy;

	UPROPERTY(EditAnywhere)
	FVector StartLocation = FVector(13595.f, -25294.f, 70000.f);

	UPROPERTY(EditAnywhere)
	FVector AllDiveLocation = FVector(330094.f, -341793.f, 70000.f);

	UPROPERTY(EditAnywhere)
	float AllDiveToEndDist = 100000.f;

	UPROPERTY(VisibleAnywhere)
	float StartToAllDiveDist;
};
