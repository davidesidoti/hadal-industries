#pragma once

#include "CoreMinimal.h"
#include "Machines/HI_MachineBase.h"
#include "HI_FabricatorMachine.generated.h"

class UHI_RecipeDefinition;
class URecipeProcessorComponent;

/**
 * Recipe-driven crafting machine. Designer assigns SelectedRecipe in the BP defaults; on BeginPlay
 * the production interval is matched to the recipe's CraftTimeSeconds. TickProduction delegates to
 * URecipeProcessorComponent::TryProduceOneCycle — all-or-nothing per cycle, no partial progress UI.
 */
UCLASS(Blueprintable)
class HADALINDUSTRIES_API AHI_FabricatorMachine : public AHI_MachineBase
{
	GENERATED_BODY()

public:
	AHI_FabricatorMachine();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Fabricator")
	UHI_RecipeDefinition* SelectedRecipe = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hadal|Fabricator")
	TObjectPtr<URecipeProcessorComponent> RecipeProcessor;

protected:
	virtual void BeginPlay() override;
	virtual void TickProduction() override;
};
