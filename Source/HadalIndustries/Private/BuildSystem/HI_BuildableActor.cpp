#include "BuildSystem/HI_BuildableActor.h"

#include "Components/StaticMeshComponent.h"

AHI_BuildableActor::AHI_BuildableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionProfileName(FName("BlockAllDynamic"));
	MeshComponent->SetMobility(EComponentMobility::Static);
}
