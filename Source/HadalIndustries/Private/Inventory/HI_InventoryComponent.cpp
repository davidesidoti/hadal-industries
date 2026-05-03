#include "Inventory/HI_InventoryComponent.h"

#include "Core/HI_Log.h"
#include "Inventory/HI_ItemDefinition.h"

UHI_InventoryComponent::UHI_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UHI_ItemDefinition* UHI_InventoryComponent::FindDefinition(FName ItemId) const
{
	for (UHI_ItemDefinition* Def : KnownItems)
	{
		if (Def && Def->ItemId == ItemId) return Def;
	}
	return nullptr;
}

int32 UHI_InventoryComponent::GetMaxStackSizeFor(FName ItemId) const
{
	if (UHI_ItemDefinition* Def = FindDefinition(ItemId))
	{
		return FMath::Max(1, Def->MaxStackSize);
	}
	// Fallback when item is unknown to this inventory: cap at 1 per slot to surface the registration gap quickly.
	return 1;
}

int32 UHI_InventoryComponent::AddStack(FName ItemId, int32 Quantity)
{
	if (ItemId.IsNone() || Quantity <= 0) return Quantity;

	const int32 MaxStack = GetMaxStackSizeFor(ItemId);
	int32 Remaining = Quantity;

	// Fill existing matching slots first.
	for (FHI_ItemStack& Slot : Slots)
	{
		if (Remaining <= 0) break;
		if (Slot.ItemId != ItemId) continue;
		const int32 Space = MaxStack - Slot.Quantity;
		if (Space <= 0) continue;
		const int32 ToAdd = FMath::Min(Space, Remaining);
		Slot.Quantity += ToAdd;
		Remaining -= ToAdd;
	}

	// Spill into new slots.
	while (Remaining > 0 && Slots.Num() < MaxSlots)
	{
		const int32 ToAdd = FMath::Min(MaxStack, Remaining);
		Slots.Emplace(ItemId, ToAdd);
		Remaining -= ToAdd;
	}

	const int32 Added = Quantity - Remaining;
	if (Added > 0)
	{
		UE_LOG(LogHadalIndustries, Log, TEXT("Inventory[%s]: added %dx %s (total %d, leftover %d)"),
			*GetNameSafe(GetOwner()), Added, *ItemId.ToString(), GetTotalQuantity(ItemId), Remaining);
		OnInventoryChanged.Broadcast();
	}
	else
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("Inventory[%s]: full, could not add %dx %s"),
			*GetNameSafe(GetOwner()), Quantity, *ItemId.ToString());
	}

	return Remaining;
}

int32 UHI_InventoryComponent::RemoveItem(FName ItemId, int32 MaxToRemove)
{
	if (ItemId.IsNone() || MaxToRemove <= 0) return 0;

	int32 Removed = 0;
	for (int32 Index = Slots.Num() - 1; Index >= 0 && Removed < MaxToRemove; --Index)
	{
		FHI_ItemStack& Slot = Slots[Index];
		if (Slot.ItemId != ItemId) continue;
		const int32 Take = FMath::Min(Slot.Quantity, MaxToRemove - Removed);
		Slot.Quantity -= Take;
		Removed += Take;
		if (Slot.Quantity <= 0)
		{
			Slots.RemoveAt(Index);
		}
	}

	if (Removed > 0)
	{
		UE_LOG(LogHadalIndustries, Log, TEXT("Inventory[%s]: removed %dx %s (total %d)"),
			*GetNameSafe(GetOwner()), Removed, *ItemId.ToString(), GetTotalQuantity(ItemId));
		OnInventoryChanged.Broadcast();
	}
	return Removed;
}

int32 UHI_InventoryComponent::GetTotalQuantity(FName ItemId) const
{
	int32 Total = 0;
	for (const FHI_ItemStack& Slot : Slots)
	{
		if (Slot.ItemId == ItemId) Total += Slot.Quantity;
	}
	return Total;
}

bool UHI_InventoryComponent::HasItem(FName ItemId, int32 MinQuantity) const
{
	return GetTotalQuantity(ItemId) >= MinQuantity;
}

int32 UHI_InventoryComponent::TransferTo(UHI_InventoryComponent* Other, FName ItemId, int32 Quantity)
{
	if (!Other || ItemId.IsNone() || Quantity <= 0) return 0;
	const int32 Removed = RemoveItem(ItemId, Quantity);
	const int32 Leftover = Other->AddStack(ItemId, Removed);
	if (Leftover > 0)
	{
		// Other inventory was full: put leftover back in this one.
		AddStack(ItemId, Leftover);
	}
	return Removed - Leftover;
}

void UHI_InventoryComponent::DebugLogContents() const
{
	UE_LOG(LogHadalIndustries, Log, TEXT("Inventory[%s] contents (%d/%d slots):"),
		*GetNameSafe(GetOwner()), Slots.Num(), MaxSlots);
	for (int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		UE_LOG(LogHadalIndustries, Log, TEXT("  [%d] %s x%d"),
			Index, *Slots[Index].ItemId.ToString(), Slots[Index].Quantity);
	}
}
