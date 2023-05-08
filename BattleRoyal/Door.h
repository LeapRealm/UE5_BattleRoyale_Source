#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

class UBoxComponent;

UENUM()
enum class EDoorState : uint8
{
	Closed,
	OpenedPlusYAxis,
	OpenedMinusYAxis
};

UCLASS()
class BATTLEROYAL_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADoor();

protected:
	virtual void BeginPlay() override;

public:
	void RotateDoor(const FVector& PlayerPos); 

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RotateDoor(bool bIsPlayerPlusY);

	UFUNCTION()
	void OpenPlusYAxisInterp(float Value);

	UFUNCTION()
	void OpenMinusYAxisInterp(float Value);
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> OverlapBox;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	// Timeline
	UPROPERTY(EditAnywhere, Category="Timeline")
	TObjectPtr<UCurveFloat> SmoothCurveDoor;
	
	UPROPERTY(VisibleAnywhere, Category="Timeline")
	TObjectPtr<UTimelineComponent> OpenPlusYAxisTimeline;

	UPROPERTY(VisibleAnywhere, Category="Timeline")
	TObjectPtr<UTimelineComponent> OpenMinusYAxisTimeline;
	
public:
	UPROPERTY(VisibleAnywhere)
	EDoorState State = EDoorState::Closed;

private:
	FOnTimelineFloat OpenPlusYAxisInterpFunction;
	FOnTimelineFloat OpenMinusYAxisInterpFunction;
};
