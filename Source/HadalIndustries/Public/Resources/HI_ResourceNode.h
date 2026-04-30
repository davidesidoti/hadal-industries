#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/HI_InteractableInterface.h"
#include "HI_ResourceNode.generated.h"

class UStaticMeshComponent;

UCLASS()
class HADALINDUSTRIES_API AHI_ResourceNode : public AActor, public IHI_InteractableInterface
{
	GENERATED_BODY()

public:
	AHI_ResourceNode();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Resource")
	FName ResourceId = TEXT("res_manganese_nodule");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Resource")
	FText DisplayName = NSLOCTEXT("HadalIndustries", "ManganeseNodule", "Manganese Nodule");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Resource", meta = (ClampMin = "0"))
	int32 RemainingYield = 5;

	virtual void OnInteract_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual bool CanBeInteractedWith_Implementation(const AActor* Interactor) const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hadal|Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;
};
