#include "Machines/HI_ExtractorMachine.h"

#include "Kismet/GameplayStatics.h"

#include "Core/HI_Log.h"
#include "Inventory/HI_InventoryComponent.h"
#include "Inventory/HI_ItemDefinition.h"
#include "Resources/HI_ResourceNode.h"

AHI_ExtractorMachine::AHI_ExtractorMachine()
{
	ProductionInterval = 5.0f;
}

void AHI_ExtractorMachine::BeginPlay()
{
	Super::BeginPlay();
	TryBindNearestNode();
}

void AHI_ExtractorMachine::TryBindNearestNode()
{
	BoundNode.Reset();

	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> Candidates;
	UGameplayStatics::GetAllActorsOfClass(World, AHI_ResourceNode::StaticClass(), Candidates);

	const FVector Origin = GetActorLocation();
	const float MaxDistSq = FMath::Square(BindRadius);

	AHI_ResourceNode* Closest = nullptr;
	float ClosestDistSq = TNumericLimits<float>::Max();

	for (AActor* Actor : Candidates)
	{
		AHI_ResourceNode* Node = Cast<AHI_ResourceNode>(Actor);
		if (!Node || Node->RemainingYield <= 0 || !Node->ItemDefinition) continue;

		const float DistSq = FVector::DistSquared(Origin, Node->GetActorLocation());
		if (DistSq <= MaxDistSq && DistSq < ClosestDistSq)
		{
			ClosestDistSq = DistSq;
			Closest = Node;
		}
	}

	if (Closest)
	{
		BoundNode = Closest;
		UE_LOG(LogHadalIndustries, Log, TEXT("Extractor %s: bound to %s (%s, %d remaining)"),
			*GetName(),
			*Closest->GetName(),
			*Closest->ItemDefinition->ItemId.ToString(),
			Closest->RemainingYield);
	}
	else
	{
		UE_LOG(LogHadalIndustries, Log, TEXT("Extractor %s: no resource node within %.0f cm"),
			*GetName(), BindRadius);
	}
}

void AHI_ExtractorMachine::TickProduction()
{
	if (!IsProductionAllowed())
	{
		// Pick the most descriptive failure state. M5/M6 will fold their checks here.
		SetState(bPowerSatisfied ? EHI_MachineState::OverPressure : EHI_MachineState::NoPower);
		return;
	}

	if (!BoundNode.IsValid())
	{
		TryBindNearestNode();
		if (!BoundNode.IsValid())
		{
			SetState(EHI_MachineState::NoBoundNode);
			return;
		}
	}

	if (!OutputInventory)
	{
		SetState(EHI_MachineState::Idle);
		return;
	}

	FName ProducedItemId;
	if (!BoundNode->TryConsumeYield(1, ProducedItemId))
	{
		// Node depleted (or unable to yield). Drop binding so the next tick re-scans.
		BoundNode.Reset();
		SetState(EHI_MachineState::NoBoundNode);
		return;
	}

	const int32 Leftover = OutputInventory->AddStack(ProducedItemId, 1);
	if (Leftover > 0)
	{
		// Output couldn't accept the unit — credit it back to the node so we don't lose yield.
		BoundNode->RemainingYield += Leftover;
		SetState(EHI_MachineState::OutputFull);
		UE_LOG(LogHadalIndustries, Log, TEXT("Extractor %s: output full, %d unit returned to node"),
			*GetName(), Leftover);
		return;
	}

	SetState(EHI_MachineState::Producing);
	UE_LOG(LogHadalIndustries, Log, TEXT("Extractor %s: +1x %s (output total %d)"),
		*GetName(),
		*ProducedItemId.ToString(),
		OutputInventory->GetTotalQuantity(ProducedItemId));
}
