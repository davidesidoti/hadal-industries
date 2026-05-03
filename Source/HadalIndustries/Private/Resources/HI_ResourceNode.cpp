#include "Resources/HI_ResourceNode.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

#include "Core/HI_Log.h"
#include "Inventory/HI_InventoryComponent.h"
#include "Inventory/HI_ItemDefinition.h"

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
	if (!ItemDefinition)
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("ResourceNode %s: no ItemDefinition assigned"), *GetName());
		return;
	}
	if (RemainingYield <= 0)
	{
		UE_LOG(LogHadalIndustries, Log, TEXT("ResourceNode %s (%s): depleted"),
			*GetName(), *ItemDefinition->ItemId.ToString());
		return;
	}

	UHI_InventoryComponent* Inventory = Interactor ? Interactor->FindComponentByClass<UHI_InventoryComponent>() : nullptr;
	if (!Inventory)
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("ResourceNode %s: interactor %s has no InventoryComponent"),
			*GetName(), Interactor ? *Interactor->GetName() : TEXT("<null>"));
		return;
	}

	const int32 Leftover = Inventory->AddStack(ItemDefinition->ItemId, YieldPerInteraction);
	const int32 Granted = YieldPerInteraction - Leftover;
	if (Granted <= 0)
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("ResourceNode %s: inventory full, no harvest"), *GetName());
		return;
	}

	RemainingYield -= 1;
	UE_LOG(LogHadalIndustries, Log, TEXT("ResourceNode %s (%s): harvested %d by %s (remaining %d)"),
		*GetName(),
		*ItemDefinition->ItemId.ToString(),
		Granted,
		*Interactor->GetName(),
		RemainingYield);
}

FText AHI_ResourceNode::GetInteractionPrompt_Implementation() const
{
	const FText Name = ItemDefinition ? ItemDefinition->DisplayName : NSLOCTEXT("HadalIndustries", "UnknownResource", "Unknown Resource");
	return FText::Format(NSLOCTEXT("HadalIndustries", "ResourcePrompt", "Harvest {0} ({1} left)"),
		Name, FText::AsNumber(RemainingYield));
}

bool AHI_ResourceNode::CanBeInteractedWith_Implementation(const AActor* Interactor) const
{
	return ItemDefinition != nullptr && RemainingYield > 0;
}
