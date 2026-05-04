#include "Machines/HI_MachineBase.h"

#include "Engine/World.h"
#include "TimerManager.h"

#include "Core/HI_Log.h"
#include "Inventory/HI_InventoryComponent.h"

TArray<TWeakObjectPtr<AHI_MachineBase>> AHI_MachineBase::ActiveMachines;

AHI_MachineBase::AHI_MachineBase()
{
	PrimaryActorTick.bCanEverTick = false;

	InputInventory = CreateDefaultSubobject<UHI_InventoryComponent>(TEXT("InputInventory"));
	OutputInventory = CreateDefaultSubobject<UHI_InventoryComponent>(TEXT("OutputInventory"));
}

void AHI_MachineBase::BeginPlay()
{
	Super::BeginPlay();
	ActiveMachines.Add(this);
	RestartProductionTimer();
}

void AHI_MachineBase::RestartProductionTimer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FTimerManager& TM = World->GetTimerManager();
	TM.ClearTimer(ProductionTimerHandle);

	const float Interval = FMath::Max(0.1f, ProductionInterval);
	TM.SetTimer(
		ProductionTimerHandle,
		this,
		&AHI_MachineBase::TickProduction,
		Interval,
		/*bLoop=*/ true,
		/*FirstDelay=*/ Interval);
}

void AHI_MachineBase::EndPlay(const EEndPlayReason::Type Reason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ProductionTimerHandle);
	}

	ActiveMachines.RemoveAll([this](const TWeakObjectPtr<AHI_MachineBase>& Entry)
	{
		return !Entry.IsValid() || Entry.Get() == this;
	});

	Super::EndPlay(Reason);
}

void AHI_MachineBase::TickProduction()
{
	// Base implementation is intentionally empty.
}

bool AHI_MachineBase::SetState(EHI_MachineState NewState)
{
	if (State == NewState) return false;
	State = NewState;
	OnMachineStateChanged.Broadcast(State);
	return true;
}

int32 AHI_MachineBase::DrainInventoryInto(UHI_InventoryComponent* Source, UHI_InventoryComponent* Target)
{
	if (!Source || !Target) return 0;

	// Snapshot the slots so TransferTo's RemoveItem doesn't invalidate iteration.
	TArray<FHI_ItemStack> Snapshot = Source->GetSlots();
	int32 Moved = 0;
	for (const FHI_ItemStack& Stack : Snapshot)
	{
		if (Stack.IsEmpty()) continue;
		Moved += Source->TransferTo(Target, Stack.ItemId, Stack.Quantity);
	}
	return Moved;
}

void AHI_MachineBase::OnInteract_Implementation(AActor* Interactor)
{
	if (!OutputInventory || !Interactor) return;

	UHI_InventoryComponent* PlayerInv = Interactor->FindComponentByClass<UHI_InventoryComponent>();
	if (!PlayerInv)
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("Machine %s: interactor %s has no InventoryComponent"),
			*GetName(), *Interactor->GetName());
		return;
	}

	const int32 Moved = DrainInventoryInto(OutputInventory, PlayerInv);
	UE_LOG(LogHadalIndustries, Log, TEXT("Machine %s: drained %d items into %s"),
		*GetName(), Moved, *Interactor->GetName());

	// Output may have just freed up — let the next production tick pick that up naturally.
}

FText AHI_MachineBase::GetInteractionPrompt_Implementation() const
{
	return NSLOCTEXT("HadalIndustries", "MachineDrainPrompt", "Drain output");
}

bool AHI_MachineBase::CanBeInteractedWith_Implementation(const AActor* Interactor) const
{
	if (!OutputInventory) return false;
	for (const FHI_ItemStack& Stack : OutputInventory->GetSlots())
	{
		if (!Stack.IsEmpty()) return true;
	}
	return false;
}

void AHI_MachineBase::OnPreDemolish(UHI_InventoryComponent* RefundTarget)
{
	if (!RefundTarget) return;

	const int32 OutMoved = DrainInventoryInto(OutputInventory, RefundTarget);
	const int32 InMoved = DrainInventoryInto(InputInventory, RefundTarget);

	if (OutMoved > 0 || InMoved > 0)
	{
		UE_LOG(LogHadalIndustries, Log, TEXT("Machine %s: demolish dumped %d (output) + %d (input) to player"),
			*GetName(), OutMoved, InMoved);
	}
}
