#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HI_InteractableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UHI_InteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class HADALINDUSTRIES_API IHI_InteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hadal|Interaction")
	void OnInteract(AActor* Interactor);
	virtual void OnInteract_Implementation(AActor* Interactor) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hadal|Interaction")
	FText GetInteractionPrompt() const;
	virtual FText GetInteractionPrompt_Implementation() const { return FText::GetEmpty(); }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hadal|Interaction")
	bool CanBeInteractedWith(const AActor* Interactor) const;
	virtual bool CanBeInteractedWith_Implementation(const AActor* Interactor) const { return true; }
};
