#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HI_ItemDefinition.generated.h"

class UTexture2D;

/**
 * Static definition of an item the player can hold or transfer.
 * Designers create one Data Asset per item type (DA_Item_Manganese, etc.).
 * See MVP_PLAN sez. 14 / 24.
 */
UCLASS(BlueprintType)
class HADALINDUSTRIES_API UHI_ItemDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Stable lowercase snake_case id used by gameplay code (es. "res_manganese_nodule"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Item")
	FName ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Item", meta = (MultiLine = "true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Item")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Item", meta = (ClampMin = "1"))
	int32 MaxStackSize = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Item")
	bool bIsRawResource = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Item")
	bool bIsCraftedComponent = false;
};
