#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/HI_Types.h"
#include "HI_InventoryComponent.generated.h"

class UHI_ItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHI_InventoryChanged);

/**
 * Slot-based inventory. Each slot is a FHI_ItemStack (ItemId + Quantity).
 * Uses MaxStackSize from the item's definition (resolved on add).
 */
UCLASS(ClassGroup = ("Hadal"), meta = (BlueprintSpawnableComponent))
class HADALINDUSTRIES_API UHI_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHI_InventoryComponent();

	/** Maximum number of distinct slots (each slot = stack of one item type). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Inventory", meta = (ClampMin = "1"))
	int32 MaxSlots = 32;

	/** Optional: a registry resolving FName -> UHI_ItemDefinition*. Designer assigns Data Assets here. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Inventory")
	TArray<UHI_ItemDefinition*> KnownItems;

	UPROPERTY(BlueprintAssignable, Category = "Hadal|Inventory")
	FHI_InventoryChanged OnInventoryChanged;

	/** Adds quantity into existing matching stacks first, then new slots. Returns leftover (couldn't fit). */
	UFUNCTION(BlueprintCallable, Category = "Hadal|Inventory")
	int32 AddStack(FName ItemId, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Hadal|Inventory")
	int32 RemoveItem(FName ItemId, int32 MaxToRemove);

	UFUNCTION(BlueprintPure, Category = "Hadal|Inventory")
	int32 GetTotalQuantity(FName ItemId) const;

	UFUNCTION(BlueprintPure, Category = "Hadal|Inventory")
	bool HasItem(FName ItemId, int32 MinQuantity) const;

	UFUNCTION(BlueprintCallable, Category = "Hadal|Inventory")
	int32 TransferTo(UHI_InventoryComponent* Other, FName ItemId, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Hadal|Inventory")
	void DebugLogContents() const;

	UFUNCTION(BlueprintCallable, Category = "Hadal|Inventory")
	const TArray<FHI_ItemStack>& GetSlots() const { return Slots; }

	/** Resolves an ItemId to its UHI_ItemDefinition via KnownItems. Returns null if not registered. */
	UFUNCTION(BlueprintPure, Category = "Hadal|Inventory")
	UHI_ItemDefinition* FindDefinition(FName ItemId) const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hadal|Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<FHI_ItemStack> Slots;

	int32 GetMaxStackSizeFor(FName ItemId) const;
};
