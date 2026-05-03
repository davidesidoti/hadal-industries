#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HI_BuildableActor.generated.h"

class UStaticMeshComponent;
class UHI_BuildableDefinition;
class UHI_InventoryComponent;

/**
 * Base class for any actor spawned by the build system. Concrete buildables are BP children
 * (BP_HI_Buildable_Foundation_Mk1, BP_HI_Buildable_Extractor_Mk1, ...).
 */
UCLASS()
class HADALINDUSTRIES_API AHI_BuildableActor : public AActor
{
	GENERATED_BODY()

public:
	AHI_BuildableActor();

	/** Stable id matching the source UHI_BuildableDefinition.BuildableId. Useful for save/load. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Buildable")
	FName BuildableId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Buildable")
	UHI_BuildableDefinition* SourceDefinition = nullptr;

	UFUNCTION(BlueprintPure, Category = "Hadal|Buildable")
	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

	/**
	 * Called by the build manager just before this actor is destroyed via demolish.
	 * Subclasses (e.g. machines) override to dump their inventories back to the player.
	 * Default no-op; foundation/static buildables don't need to override.
	 */
	virtual void OnPreDemolish(UHI_InventoryComponent* RefundTarget) {}

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hadal|Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;
};
