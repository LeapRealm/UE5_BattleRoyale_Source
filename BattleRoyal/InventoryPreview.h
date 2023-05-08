#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldItem.h"
#include "InventoryPreview.generated.h"

class USpotLightComponent;
UCLASS()
class BATTLEROYAL_API AInventoryPreview : public AActor
{
	GENERATED_BODY()
	
public:	
	AInventoryPreview();
	
	virtual void BeginPlay() override;
	
public:
	void ResetMeshes();
	void SetWeaponMesh(EWeaponType WeaponType, USkeletalMesh* NewMesh);
	void SetArmorMesh(EArmorType ArmorType, USkeletalMesh* NewMesh, USkeletalMesh* NewAttachmentMesh = nullptr);
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneCaptureComponent2D> CaptureComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> HeadMeshComponent;

private:
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> ShirtMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> PantsMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> BootsMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> HandsMeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> VestMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> BackpackMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> BackpackStringMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> HelmetMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMeshComponent> HelmetOutfitMeshComponent;

	// Default Mesh
	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMesh> DefaultHeadMesh;

	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMesh> DefaultShirtMesh;

	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMesh> DefaultHandsMesh;

	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMesh> DefaultPantsMesh;

	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<USkeletalMesh> DefaultBootsMesh;

	// Animation
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> IdleMontage;

	// Light
	UPROPERTY(VisibleAnywhere, Category="Light")
	TObjectPtr<USpotLightComponent> SpotLightLeft;
	
	UPROPERTY(VisibleAnywhere, Category="Light")
	TObjectPtr<USpotLightComponent> SpotLightRight;
	
	UPROPERTY(VisibleAnywhere, Category="Light")
	TObjectPtr<USpotLightComponent> SpotLightFront;
};
