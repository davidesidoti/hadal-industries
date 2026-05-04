#include "Recipes/HI_RecipeProcessorComponent.h"

#include "Core/HI_Log.h"
#include "Core/HI_Types.h"
#include "Inventory/HI_InventoryComponent.h"
#include "Recipes/HI_RecipeDefinition.h"

URecipeProcessorComponent::URecipeProcessorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool URecipeProcessorComponent::CanProduce(const UHI_RecipeDefinition* Recipe, const UHI_InventoryComponent* Input, const UHI_InventoryComponent* Output) const
{
	if (!Recipe || !Input || !Output) return false;

	for (const FHI_ItemStack& Stack : Recipe->Inputs)
	{
		if (Stack.IsEmpty()) continue;
		if (!Input->HasItem(Stack.ItemId, Stack.Quantity)) return false;
	}

	// Output capacity check is best-effort: AddStack will spill into new slots up to MaxSlots,
	// so we only block when an Output product item isn't even registered for stack-size lookup.
	// (Empty outputs are valid — recipe might be "destroy items" only.)
	return true;
}

bool URecipeProcessorComponent::TryProduceOneCycle(const UHI_RecipeDefinition* Recipe, UHI_InventoryComponent* Input, UHI_InventoryComponent* Output)
{
	if (!CanProduce(Recipe, Input, Output)) return false;

	// Consume inputs first so a partial output failure can't double-spend.
	for (const FHI_ItemStack& Stack : Recipe->Inputs)
	{
		if (Stack.IsEmpty()) continue;
		const int32 Removed = Input->RemoveItem(Stack.ItemId, Stack.Quantity);
		if (Removed < Stack.Quantity)
		{
			UE_LOG(LogHadalIndustries, Warning, TEXT("RecipeProcessor: failed to remove %dx %s (only %d removed) — recipe %s aborted mid-consume"),
				Stack.Quantity, *Stack.ItemId.ToString(), Removed, *Recipe->RecipeId.ToString());
			return false;
		}
	}

	// Deposit outputs.
	for (const FHI_ItemStack& Stack : Recipe->Outputs)
	{
		if (Stack.IsEmpty()) continue;
		const int32 Leftover = Output->AddStack(Stack.ItemId, Stack.Quantity);
		if (Leftover > 0)
		{
			UE_LOG(LogHadalIndustries, Warning, TEXT("RecipeProcessor: %dx %s leftover from recipe %s (output full)"),
				Leftover, *Stack.ItemId.ToString(), *Recipe->RecipeId.ToString());
		}
	}

	UE_LOG(LogHadalIndustries, Log, TEXT("RecipeProcessor: %s cycle complete on %s"),
		*Recipe->RecipeId.ToString(), *GetNameSafe(GetOwner()));
	return true;
}
