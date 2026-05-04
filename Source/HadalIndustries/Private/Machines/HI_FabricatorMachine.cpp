#include "Machines/HI_FabricatorMachine.h"

#include "Core/HI_Log.h"
#include "Inventory/HI_InventoryComponent.h"
#include "Recipes/HI_RecipeDefinition.h"
#include "Recipes/HI_RecipeProcessorComponent.h"

AHI_FabricatorMachine::AHI_FabricatorMachine()
{
	RecipeProcessor = CreateDefaultSubobject<URecipeProcessorComponent>(TEXT("RecipeProcessor"));
	ProductionInterval = 4.0f;
}

void AHI_FabricatorMachine::BeginPlay()
{
	if (SelectedRecipe)
	{
		ProductionInterval = FMath::Max(0.1f, SelectedRecipe->CraftTimeSeconds);
	}
	Super::BeginPlay();

	if (!SelectedRecipe)
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("Fabricator %s: no SelectedRecipe assigned — will idle"),
			*GetName());
	}
}

void AHI_FabricatorMachine::TickProduction()
{
	if (!IsProductionAllowed())
	{
		const EHI_MachineState NewState = bPowerSatisfied ? EHI_MachineState::OverPressure : EHI_MachineState::NoPower;
		if (SetState(NewState))
		{
			UE_LOG(LogHadalIndustries, Log, TEXT("Fabricator %s: paused (%s)"),
				*GetName(),
				NewState == EHI_MachineState::NoPower ? TEXT("NoPower") : TEXT("OverPressure"));
		}
		return;
	}

	if (!SelectedRecipe || !RecipeProcessor || !InputInventory || !OutputInventory)
	{
		SetState(EHI_MachineState::Idle);
		return;
	}

	if (!RecipeProcessor->CanProduce(SelectedRecipe, InputInventory, OutputInventory))
	{
		// Distinguish "missing inputs" (Idle) from "cannot deposit outputs" — no fine-grained API for the
		// second case in M4b, so we coalesce both into Idle. M5+ machine UI can surface details.
		if (SetState(EHI_MachineState::Idle))
		{
			UE_LOG(LogHadalIndustries, Log, TEXT("Fabricator %s: idle, recipe %s requirements not met"),
				*GetName(), *SelectedRecipe->RecipeId.ToString());
		}
		return;
	}

	if (RecipeProcessor->TryProduceOneCycle(SelectedRecipe, InputInventory, OutputInventory))
	{
		SetState(EHI_MachineState::Producing);
	}
	else
	{
		SetState(EHI_MachineState::Idle);
	}
}
