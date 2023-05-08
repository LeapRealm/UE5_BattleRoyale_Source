#include "InventoryPreview.h"

#include "WorldItem.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpotLightComponent.h"

AInventoryPreview::AInventoryPreview()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture"));
	CaptureComponent->SetupAttachment(RootComponent);
	CaptureComponent->SetRelativeLocation(FVector(-320.f, 0.f, 145.f));
	CaptureComponent->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
	CaptureComponent->FOVAngle = 20.f;
	
	// Mesh
	HeadMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	HeadMeshComponent->SetupAttachment(RootComponent);
	HeadMeshComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Military_Head1(TEXT("/Script/Engine.SkeletalMesh'/Game/BattleRoyal/Characters/Soldier/Mesh/Body/SK_Military_Head1.SK_Military_Head1'"));
	if (SK_Military_Head1.Succeeded())
	{
		DefaultHeadMesh = SK_Military_Head1.Object;
		HeadMeshComponent->SetSkeletalMesh(DefaultHeadMesh);
	}

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	WeaponMeshComponent->SetupAttachment(HeadMeshComponent);
	
	ShirtMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shirt"));
	ShirtMeshComponent->SetupAttachment(HeadMeshComponent);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Torso_Male(TEXT("/Script/Engine.SkeletalMesh'/Game/BattleRoyal/Characters/Soldier/Mesh/Rare/Torso_Male.Torso_Male'"));
	if (Torso_Male.Succeeded())
	{
		DefaultShirtMesh = Torso_Male.Object;
		ShirtMeshComponent->SetSkeletalMesh(DefaultShirtMesh);
	}

	HandsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands"));
	HandsMeshComponent->SetupAttachment(HeadMeshComponent);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Military_Hands_Male(TEXT("/Script/Engine.SkeletalMesh'/Game/BattleRoyal/Characters/Soldier/Mesh/Body/SK_Military_Hands_Male.SK_Military_Hands_Male'"));
	if (SK_Military_Hands_Male.Succeeded())
	{
		DefaultHandsMesh = SK_Military_Hands_Male.Object;
		HandsMeshComponent->SetSkeletalMesh(DefaultHandsMesh);
	}
	
	PantsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Pants"));
	PantsMeshComponent->SetupAttachment(HeadMeshComponent);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Legs_Male(TEXT("/Script/Engine.SkeletalMesh'/Game/BattleRoyal/Characters/Soldier/Mesh/Rare/Legs_Male.Legs_Male'"));
	if (Legs_Male.Succeeded())
	{
		DefaultPantsMesh = Legs_Male.Object;
		PantsMeshComponent->SetSkeletalMesh(DefaultPantsMesh);
	}
	
	BootsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Boots"));
	BootsMeshComponent->SetupAttachment(HeadMeshComponent);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Military_Foot_Male(TEXT("/Script/Engine.SkeletalMesh'/Game/BattleRoyal/Characters/Soldier/Mesh/Body/SK_Military_Foot_Male.SK_Military_Foot_Male'"));
	if (SK_Military_Foot_Male.Succeeded())
	{
		DefaultBootsMesh = SK_Military_Foot_Male.Object;
		BootsMeshComponent->SetSkeletalMesh(DefaultBootsMesh);
	}
	
	VestMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Vest"));
	VestMeshComponent->SetupAttachment(HeadMeshComponent);

	BackpackMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Backpack"));
	BackpackMeshComponent->SetupAttachment(HeadMeshComponent);
	
	BackpackStringMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackpackString"));
	BackpackStringMeshComponent->SetupAttachment(HeadMeshComponent);

	HelmetMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helmet"));
	HelmetMeshComponent->SetupAttachment(HeadMeshComponent);

	HelmetOutfitMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetOutfit"));
	HelmetOutfitMeshComponent->SetupAttachment(HeadMeshComponent);

	// Animation
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MM_Preview_Idle_Montage(TEXT("/Script/Engine.AnimMontage'/Game/BattleRoyal/Characters/Animations/Locomotion/Unarmed/MM_Preview_Idle_Montage.MM_Preview_Idle_Montage'"));
	if (MM_Preview_Idle_Montage.Succeeded())
		IdleMontage = MM_Preview_Idle_Montage.Object;
	
	HeadMeshComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	HeadMeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	
	ShirtMeshComponent->SetLeaderPoseComponent(HeadMeshComponent);
	PantsMeshComponent->SetLeaderPoseComponent(HeadMeshComponent);
	BootsMeshComponent->SetLeaderPoseComponent(HeadMeshComponent);
	HandsMeshComponent->SetLeaderPoseComponent(HeadMeshComponent);
	VestMeshComponent->SetLeaderPoseComponent(HeadMeshComponent);
	BackpackMeshComponent->SetLeaderPoseComponent(HeadMeshComponent);
	BackpackStringMeshComponent->SetLeaderPoseComponent(HeadMeshComponent);
	HelmetMeshComponent->SetLeaderPoseComponent(HeadMeshComponent);
	HelmetOutfitMeshComponent->SetLeaderPoseComponent(HeadMeshComponent);

	// Light
	SpotLightLeft = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightLeft"));
	SpotLightLeft->SetRelativeLocation(FVector(-6.f, -246.f, 242.f));
	SpotLightLeft->SetRelativeRotation(FRotator(-30.f, 84.5f, 5.f));
	SpotLightLeft->SetupAttachment(RootComponent);
	
	SpotLightFront = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightFront"));
	SpotLightFront->SetRelativeLocation(FVector(-275.8f, 59.f, 91.f));
	SpotLightFront->SetRelativeRotation(FRotator(-1.f, -11.5f, -16.f));
	SpotLightFront->SetupAttachment(RootComponent);
}

void AInventoryPreview::BeginPlay()
{
	Super::BeginPlay();
	
	HeadMeshComponent->PlayAnimation(IdleMontage, true);
}

void AInventoryPreview::ResetMeshes()
{
	SetWeaponMesh(EWeaponType::None, nullptr);
	
	SetArmorMesh(EArmorType::Helmet, nullptr);
	SetArmorMesh(EArmorType::Shirt, nullptr);
	SetArmorMesh(EArmorType::Vest, nullptr);
	SetArmorMesh(EArmorType::Backpack, nullptr);
	SetArmorMesh(EArmorType::Pants, nullptr);
	SetArmorMesh(EArmorType::Boots, nullptr);
}

void AInventoryPreview::SetWeaponMesh(EWeaponType WeaponType, USkeletalMesh* NewMesh)
{
	WeaponMeshComponent->SetSkeletalMesh(NewMesh);
	
	FAttachmentTransformRules Rules { EAttachmentRule::SnapToTarget, true };
	
	switch (WeaponType)
	{
	case EWeaponType::Pistol:
		WeaponMeshComponent->AttachToComponent(HeadMeshComponent, Rules, TEXT("pistol_socket"));
		break;
		
	case EWeaponType::Rifle:
		WeaponMeshComponent->AttachToComponent(HeadMeshComponent, Rules, TEXT("rifle_socket"));
		break;
		
	case EWeaponType::Shotgun:
		WeaponMeshComponent->AttachToComponent(HeadMeshComponent, Rules, TEXT("shotgun_socket"));
		break;
	}
}

void AInventoryPreview::SetArmorMesh(EArmorType ArmorType, USkeletalMesh* NewMesh, USkeletalMesh* NewAttachmentMesh)
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
}
