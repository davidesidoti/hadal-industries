#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HI_RecipeProcessorComponent.generated.h"

class UHI_RecipeDefinition;
class UHI_InventoryComponent;

/**
 * Stateless recipe runner. Holds no progress of its own — the owning machine drives timing through
 * its TickProduction loop, and each successful TryProduceOneCycle consumes inputs and emits outputs
 * atomically. M4b uses this in AHI_FabricatorMachine.
 */
UCLASS(ClassGroup = ("Hadal"), meta = (BlueprintSpawnableComponent))
class HADALINDUSTRIES_API URecipeProcessorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URecipeProcessorComponent();

	/** True when Input has every ingredient AND Output has room for every product (per stack). */
	UFUNCTION(BlueprintPure, Category = "Hadal|Recipe")
	bool CanProduce(const UHI_RecipeDefinition* Recipe, const UHI_InventoryComponent* Input, const UHI_InventoryComponent* Output) const;

	/**
	 * Atomically consume Recipe.Inputs from Input and add Recipe.Outputs to Output.
	 * Returns true if the cycle ran. If outputs partially fail to fit, leftovers are credited back
	 * to the source inventory through the standard AddStack path.
	 */
	UFUNCTION(BlueprintCallable, Category = "Hadal|Recipe")
	bool TryProduceOneCycle(const UHI_RecipeDefinition* Recipe, UHI_InventoryComponent* Input, UHI_InventoryComponent* Output);
};
