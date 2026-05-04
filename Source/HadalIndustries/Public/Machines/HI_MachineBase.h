#pragma once

#include "CoreMinimal.h"
#include "BuildSystem/HI_BuildableActor.h"
#include "Core/HI_Types.h"
#include "Interaction/HI_InteractableInterface.h"
#include "HI_MachineBase.generated.h"

class UHI_InventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMachineStateChanged, EHI_MachineState, NewState);

/**
 * Base class for any placed machine. Owns an Input and Output inventory plus a fixed-interval
 * production timer. Subclasses override TickProduction() to implement what happens each cycle.
 *
 * Player interaction (E) drains the Output inventory into the interactor's inventory.
 * Demolish via the build manager dumps both Input and Output back to the player via OnPreDemolish.
 *
 * Power and pressure are stubbed as plain bools for M4; M5 / M6 will swap the implementation
 * behind the IsProductionAllowed() accessor without touching call sites.
 */
UCLASS(Abstract, Blueprintable)
class HADALINDUSTRIES_API AHI_MachineBase : public AHI_BuildableActor, public IHI_InteractableInterface
{
	GENERATED_BODY()

public:
	AHI_MachineBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hadal|Machine")
	TObjectPtr<UHI_InventoryComponent> InputInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hadal|Machine")
	TObjectPtr<UHI_InventoryComponent> OutputInventory;

	/** Seconds between production ticks. Subclasses can override the default in their constructor. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hadal|Machine", meta = (ClampMin = "0.1"))
	float ProductionInterval = 5.0f;

	UPROPERTY(BlueprintAssignable, Category = "Hadal|Machine")
	FOnMachineStateChanged OnMachineStateChanged;

	UFUNCTION(BlueprintPure, Category = "Hadal|Machine")
	EHI_MachineState GetState() const { return State; }

	// IHI_InteractableInterface — drain Output into Interactor's inventory (placeholder for proper Machine UI).
	virtual void OnInteract_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual bool CanBeInteractedWith_Implementation(const AActor* Interactor) const override;

	// AHI_BuildableActor override — refund both inventories on demolish.
	virtual void OnPreDemolish(UHI_InventoryComponent* RefundTarget) override;

	/** All currently-alive machines. Registered in BeginPlay, unregistered in EndPlay. */
	static TArray<TWeakObjectPtr<AHI_MachineBase>> ActiveMachines;

	/** Clears any running production timer and arms a new one using the current ProductionInterval. */
	UFUNCTION(BlueprintCallable, Category = "Hadal|Machine")
	void RestartProductionTimer();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

	/** Called every ProductionInterval seconds. Base no-op; subclasses override. */
	virtual void TickProduction();

	/** Single accessor — M5/M6 swap the implementation here without touching subclass call sites. */
	bool IsProductionAllowed() const { return bPowerSatisfied && bPressureSafe; }

	/** Returns true when the state actually changed — useful to gate one-shot transition logs. */
	bool SetState(EHI_MachineState NewState);

	/** Drain every non-empty slot of Source into Target. Returns total quantity moved. */
	static int32 DrainInventoryInto(UHI_InventoryComponent* Source, UHI_InventoryComponent* Target);

	/** Stub flags — replaced by UHI_PowerConsumerComponent / UHI_PressureComponent in later milestones. */
	UPROPERTY(EditDefaultsOnly, Category = "Hadal|Machine|Stub")
	bool bPowerSatisfied = true;

	UPROPERTY(EditDefaultsOnly, Category = "Hadal|Machine|Stub")
	bool bPressureSafe = true;

private:
	FTimerHandle ProductionTimerHandle;
	EHI_MachineState State = EHI_MachineState::Idle;
};
