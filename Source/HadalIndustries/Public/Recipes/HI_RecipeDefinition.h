#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/HI_Types.h"
#include "HI_RecipeDefinition.generated.h"

/**
 * Static recipe definition. Designers create one Data Asset per recipe (DA_Recipe_PressurePlateMk1, ...).
 * See MVP_PLAN sez. 15 / 24.
 *
 * RequiredMachineType is matched against AHI_MachineBase::MachineType (FName) so a recipe can be
 * restricted to a fabricator or a smelter without coupling the recipe asset to a concrete C++ class.
 */
UCLASS(BlueprintType)
class HADALINDUSTRIES_API UHI_RecipeDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Recipe")
	FName RecipeId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Recipe")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Recipe", meta = (MultiLine = "true"))
	FText Description;

	/** Items consumed from the machine's InputInventory at craft start. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Recipe")
	TArray<FHI_ItemStack> Inputs;

	/** Items deposited into the machine's OutputInventory on craft completion. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Recipe")
	TArray<FHI_ItemStack> Outputs;

	/** Seconds from craft start to completion. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Recipe", meta = (ClampMin = "0.1"))
	float CraftTimeSeconds = 4.0f;

	/** Free-form tag that machines compare against their MachineType to filter recipes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Recipe")
	FName RequiredMachineType;
};
