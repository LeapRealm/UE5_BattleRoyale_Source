#include "CompassBarWidget.h"

#include "Components/Image.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UCompassBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PinIconImage->SetVisibility(ESlateVisibility::Hidden);

	PinIconMaterial = PinIconImage->GetDynamicMaterial();
	CompassMaterial = CompassImage->GetDynamicMaterial();
}

void UCompassBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (CameraManager == nullptr)
		return;
	
	float Yaw = CameraManager->GetCameraRotation().Yaw / 360.f;
	CompassMaterial->SetScalarParameterValue(TEXT("Yaw"), Yaw);

	if (bIsTargetSet)
	{
		APawn* Pawn = GetOwningPlayerPawn();
		if (IsValid(Pawn) == false)
			return;
		
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), TargetWorldPos);
		PinIconMaterial->SetScalarParameterValue("Yaw", Yaw - Rotation.Yaw / 360.f);
	}
}

void UCompassBarWidget::ShowPinIcon(const FVector& WorldPos)
{
	bIsTargetSet = true;
	TargetWorldPos = WorldPos;
	PinIconImage->SetVisibility(ESlateVisibility::Visible);
}

void UCompassBarWidget::HidePinIcon()
{
	bIsTargetSet = false;
	TargetWorldPos = FVector::ZeroVector;
	PinIconImage->SetVisibility(ESlateVisibility::Hidden);
}
