#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HI_BuildableActor.generated.h"

class UStaticMeshComponent;
class UHI_BuildableDefinition;

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

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hadal|Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;
};
