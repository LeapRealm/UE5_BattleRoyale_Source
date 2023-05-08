#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElectricField.generated.h"

UCLASS()
class BATTLEROYAL_API AElectricField : public AActor
{
	GENERATED_BODY()
	
public:	
	AElectricField();

protected:
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

public:
	UPROPERTY(EditAnywhere)
	float Damage = 10.f;
};
