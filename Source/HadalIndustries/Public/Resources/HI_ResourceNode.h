#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/HI_InteractableInterface.h"
#include "HI_ResourceNode.generated.h"

class UStaticMeshComponent;
class UHI_ItemDefinition;

UCLASS()
class HADALINDUSTRIES_API AHI_ResourceNode : public AActor, public IHI_InteractableInterface
{
	GENERATED_BODY()

public:
	AHI_ResourceNode();

	/** Item produced when this node is harvested. Designer assigns a UHI_ItemDefinition Data Asset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Resource")
	UHI_ItemDefinition* ItemDefinition = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Resource", meta = (ClampMin = "0"))
	int32 RemainingYield = 5;

	/** Quantity of the item produced per successful interaction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Resource", meta = (ClampMin = "1"))
	int32 YieldPerInteraction = 1;

	virtual void OnInteract_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual bool CanBeInteractedWith_Implementation(const AActor* Interactor) const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hadal|Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;
};
