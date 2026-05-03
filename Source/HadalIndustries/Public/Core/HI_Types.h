#pragma once

#include "CoreMinimal.h"
#include "HI_Types.generated.h"

UENUM(BlueprintType)
enum class EHI_PressureTier : uint8
{
	Tier0	UMETA(DisplayName = "Tier 0 (0-300m)"),
	Tier1	UMETA(DisplayName = "Tier 1 (300-800m)"),
	Tier2	UMETA(DisplayName = "Tier 2 (800-1200m)")
};

UENUM(BlueprintType)
enum class EHI_MachineState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	Producing		UMETA(DisplayName = "Producing"),
	OutputFull		UMETA(DisplayName = "Output Full"),
	NoBoundNode		UMETA(DisplayName = "No Bound Node"),
	NoPower			UMETA(DisplayName = "No Power"),
	OverPressure	UMETA(DisplayName = "Over Pressure")
};

USTRUCT(BlueprintType)
struct HADALINDUSTRIES_API FHI_ItemStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Inventory")
	FName ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Inventory", meta = (ClampMin = "0"))
	int32 Quantity = 0;

	FHI_ItemStack() = default;

	FHI_ItemStack(FName InItemId, int32 InQuantity)
		: ItemId(InItemId)
		, Quantity(InQuantity)
	{}

	bool IsEmpty() const { return ItemId.IsNone() || Quantity <= 0; }
};
