#pragma once

#include "CoreMinimal.h"
#include "Machines/HI_MachineBase.h"
#include "HI_ExtractorMachine.generated.h"

class AHI_ResourceNode;

/**
 * Bound-to-node extractor: scans for the closest AHI_ResourceNode within BindRadius at BeginPlay,
 * then on each ProductionInterval consumes one yield from the bound node and adds the produced item
 * to OutputInventory. Auto-rebinds in TickProduction when the bound node becomes invalid (depleted,
 * destroyed, or none was found at placement time).
 */
UCLASS(Blueprintable)
class HADALINDUSTRIES_API AHI_ExtractorMachine : public AHI_MachineBase
{
	GENERATED_BODY()

public:
	AHI_ExtractorMachine();

	/** Maximum distance (cm) to a ResourceNode that the extractor will bind to. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hadal|Extractor", meta = (ClampMin = "1.0"))
	float BindRadius = 300.0f;

protected:
	virtual void BeginPlay() override;
	virtual void TickProduction() override;

private:
	void TryBindNearestNode();

	UPROPERTY()
	TWeakObjectPtr<AHI_ResourceNode> BoundNode;
};
