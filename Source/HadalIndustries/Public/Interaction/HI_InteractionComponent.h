#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HI_InteractionComponent.generated.h"

class AActor;

UCLASS(ClassGroup = ("Hadal"), meta = (BlueprintSpawnableComponent))
class HADALINDUSTRIES_API UHI_InteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHI_InteractionComponent();

	UFUNCTION(BlueprintCallable, Category = "Hadal|Interaction")
	bool TryInteract();

	UFUNCTION(BlueprintCallable, Category = "Hadal|Interaction")
	AActor* QueryFocusedInteractable() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Interaction", meta = (ClampMin = "10.0"))
	float TraceDistance = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Interaction")
	bool bDrawDebugTrace = false;

private:
	bool TraceForward(FHitResult& OutHit) const;
};
