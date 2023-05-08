#include "ItemSpawnBoxComponent.h"

UItemSpawnBoxComponent::UItemSpawnBoxComponent()
{
	BoxExtent = FVector(200.f, 200.f, 0.f);
	LineThickness = 5.f;
	ShapeColor = FColor::Green;
	UPrimitiveComponent::SetCollisionProfileName("NoCollision");
}
