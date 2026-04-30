#include "Resources/HI_ResourceNode.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

#include "Core/HI_Log.h"

AHI_ResourceNode::AHI_ResourceNode()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionProfileName(FName("BlockAllDynamic"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMesh.Object);
	}
}

void AHI_ResourceNode::OnInteract_Implementation(AActor* Interactor)
{
	if (RemainingYield <= 0)
	{
		UE_LOG(LogHadalIndustries, Log, TEXT("ResourceNode %s: depleted"), *ResourceId.ToString());
		return;
	}

	--RemainingYield;
	UE_LOG(LogHadalIndustries, Log, TEXT("ResourceNode %s: harvested by %s (remaining: %d)"),
		*ResourceId.ToString(),
		Interactor ? *Interactor->GetName() : TEXT("<null>"),
		RemainingYield);
}

FText AHI_ResourceNode::GetInteractionPrompt_Implementation() const
{
	return FText::Format(NSLOCTEXT("HadalIndustries", "ResourcePrompt", "Harvest {0} ({1} left)"),
		DisplayName, FText::AsNumber(RemainingYield));
}

bool AHI_ResourceNode::CanBeInteractedWith_Implementation(const AActor* Interactor) const
{
	return RemainingYield > 0;
}
