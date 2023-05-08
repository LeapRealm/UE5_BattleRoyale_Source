#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "ItemSpawnBoxComponent.generated.h"

UCLASS(meta=(DisplayName="Item Spawn Box", BlueprintSpawnableComponent))
class BATTLEROYAL_API UItemSpawnBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UItemSpawnBoxComponent();
};
