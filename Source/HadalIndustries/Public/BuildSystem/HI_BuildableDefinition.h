#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/HI_Types.h"
#include "HI_BuildableDefinition.generated.h"

class AHI_BuildableActor;
class UStaticMesh;

/**
 * Static definition of a buildable. Designers create one Data Asset per buildable type
 * (DA_Buildable_FoundationMk1, DA_Buildable_ExtractorMk1, ...).
 * MVP_PLAN sez. 24 / 11.
 */
UCLASS(BlueprintType)
class HADALINDUSTRIES_API UHI_BuildableDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Buildable")
	FName BuildableId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Buildable")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Buildable", meta = (MultiLine = "true"))
	FText Description;

	/** Class spawned on confirm. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Buildable")
	TSubclassOf<AHI_BuildableActor> ActorClass;

	/** Items consumed from the player's inventory on confirm. Empty = free. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Buildable")
	TArray<FHI_ItemStack> BuildCost;

	/** Mesh shown by the ghost during placement. Should match the spawned actor's mesh visually. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Buildable")
	UStaticMesh* PreviewMesh = nullptr;

	/** Optional scale applied to the preview mesh (useful when ActorClass uses a non-uniform scale). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Buildable")
	FVector PreviewScale = FVector(1.f, 1.f, 1.f);
};
