#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/HI_Types.h"
#include "HI_BuildManagerComponent.generated.h"

class UHI_BuildableDefinition;
class UHI_InventoryComponent;
class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class AHI_BuildableActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHI_BuildModeChanged, bool, bActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHI_CurrentBuildableChanged, UHI_BuildableDefinition*, NewBuildable);

/**
 * Build manager attached to the player. Handles ghost preview, validation, cost consumption,
 * and confirmed spawning of UHI_BuildableActor instances.
 *
 * Tick is enabled only while build mode is active.
 */
UCLASS(ClassGroup = ("Hadal"), meta = (BlueprintSpawnableComponent))
class HADALINDUSTRIES_API UHI_BuildManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHI_BuildManagerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Build")
	TArray<UHI_BuildableDefinition*> AvailableBuildables;

	/** Translucent material used by the ghost. Should expose a Color vector parameter. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Build")
	UMaterialInterface* GhostMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Build", meta = (ClampMin = "50.0"))
	float MaxPlacementDistance = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Build", meta = (ClampMin = "0.1"))
	float RotationStepDegrees = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Build", meta = (ClampMin = "1.0"))
	float GridSize = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Build")
	bool bSnapEnabled = true;

	/** Fraction of BuildCost refunded when demolishing (0..1). Round down per stack, min 1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Build", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DemolishRefundFraction = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Build")
	FName GhostColorParameter = FName("Color");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Build")
	FLinearColor ValidColor = FLinearColor(0.f, 1.f, 0.f, 0.4f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Build")
	FLinearColor InvalidColor = FLinearColor(1.f, 0.f, 0.f, 0.4f);

	UPROPERTY(BlueprintAssignable, Category = "Hadal|Build")
	FHI_BuildModeChanged OnBuildModeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Hadal|Build")
	FHI_CurrentBuildableChanged OnCurrentBuildableChanged;

	UFUNCTION(BlueprintCallable, Category = "Hadal|Build")
	void ToggleBuildMode();

	UFUNCTION(BlueprintCallable, Category = "Hadal|Build")
	void SetBuildModeActive(bool bNewActive);

	UFUNCTION(BlueprintCallable, Category = "Hadal|Build")
	void CycleBuildable(int32 Direction);

	UFUNCTION(BlueprintCallable, Category = "Hadal|Build")
	void RotatePreview(float Direction);

	UFUNCTION(BlueprintCallable, Category = "Hadal|Build")
	bool TryConfirmPlace();

	UFUNCTION(BlueprintCallable, Category = "Hadal|Build")
	void CancelPlacement();

	UFUNCTION(BlueprintCallable, Category = "Hadal|Build")
	void ToggleSnap();

	/** Toggle demolish mode (mutually exclusive with build mode). LMB while active = demolish targeted. */
	UFUNCTION(BlueprintCallable, Category = "Hadal|Build")
	void ToggleDemolishMode();

	UFUNCTION(BlueprintPure, Category = "Hadal|Build")
	bool IsDemolishModeActive() const { return bDemolishActive; }

	/** Trace forward and demolish the targeted AHI_BuildableActor (if any). Refunds DemolishRefundFraction of cost. */
	UFUNCTION(BlueprintCallable, Category = "Hadal|Build")
	bool DemolishTargeted();

	UFUNCTION(BlueprintPure, Category = "Hadal|Build")
	bool IsBuildModeActive() const { return bActive; }

	UFUNCTION(BlueprintPure, Category = "Hadal|Build")
	UHI_BuildableDefinition* GetCurrentBuildable() const;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool bActive = false;
	bool bDemolishActive = false;
	int32 CurrentIndex = 0;
	float CurrentYaw = 0.0f;
	bool bLastValid = false;

	UPROPERTY()
	class AHI_BuildableActor* CurrentDemolishTarget = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* DemolishHighlightMID = nullptr;

	UPROPERTY()
	AActor* GhostActor = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* GhostMID = nullptr;

	/** Original materials saved per mesh of the currently-highlighted demolish target, restored on clear. */
	TMap<TWeakObjectPtr<class UMeshComponent>, TArray<TObjectPtr<class UMaterialInterface>>> CachedTargetMaterials;

	void SpawnGhost();
	void DestroyGhost();
	bool UpdateGhostTransform();
	void UpdateGhostColor(bool bValid);

	AHI_BuildableActor* TraceForBuildable() const;
	void UpdateDemolishHighlight();
	void ApplyHighlight(AHI_BuildableActor* Target);
	void ClearHighlight(AHI_BuildableActor* Target);

	/** Returns a forward camera ray hit point. If no hit, returns endpoint of the ray. */
	FVector ComputePlacementLocation() const;

	bool ValidatePlacement(const FTransform& Xf, FString& OutReason) const;
	bool HasMaterials(const TArray<FHI_ItemStack>& Cost, FString& OutReason) const;
	void ConsumeMaterials(const TArray<FHI_ItemStack>& Cost);

	UHI_InventoryComponent* GetOwnerInventory() const;
};
