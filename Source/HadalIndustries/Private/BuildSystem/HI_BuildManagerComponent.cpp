#include "BuildSystem/HI_BuildManagerComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/MeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

#include "BuildSystem/HI_BuildableActor.h"
#include "BuildSystem/HI_BuildableDefinition.h"
#include "Core/HI_Log.h"
#include "Inventory/HI_InventoryComponent.h"

UHI_BuildManagerComponent::UHI_BuildManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UHI_BuildManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyGhost();
	ClearHighlight(CurrentDemolishTarget);
	CurrentDemolishTarget = nullptr;
	Super::EndPlay(EndPlayReason);
}

AHI_BuildableActor* UHI_BuildManagerComponent::TraceForBuildable() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return nullptr;

	UCameraComponent* Camera = OwnerPawn->FindComponentByClass<UCameraComponent>();
	const FVector Start = Camera ? Camera->GetComponentLocation() : OwnerPawn->GetActorLocation();
	const FVector Forward = Camera ? Camera->GetForwardVector() : OwnerPawn->GetActorForwardVector();
	const FVector End = Start + Forward * MaxPlacementDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(HI_DemolishTrace), false, OwnerPawn);
	Params.AddIgnoredActor(GhostActor);

	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params)) return nullptr;
	return Cast<AHI_BuildableActor>(Hit.GetActor());
}

void UHI_BuildManagerComponent::UpdateDemolishHighlight()
{
	AHI_BuildableActor* NewTarget = TraceForBuildable();
	if (NewTarget == CurrentDemolishTarget) return;

	ClearHighlight(CurrentDemolishTarget);
	ApplyHighlight(NewTarget);
	CurrentDemolishTarget = NewTarget;
}

void UHI_BuildManagerComponent::ApplyHighlight(AHI_BuildableActor* Target)
{
	if (!Target || !GhostMaterial) return;

	if (!DemolishHighlightMID)
	{
		DemolishHighlightMID = UMaterialInstanceDynamic::Create(GhostMaterial, this);
		if (DemolishHighlightMID)
		{
			DemolishHighlightMID->SetVectorParameterValue(GhostColorParameter, InvalidColor);
		}
	}
	if (!DemolishHighlightMID) return;

	TArray<UMeshComponent*> Meshes;
	Target->GetComponents<UMeshComponent>(Meshes);
	for (UMeshComponent* Mesh : Meshes)
	{
		const int32 NumSlots = Mesh->GetNumMaterials();
		TArray<TObjectPtr<UMaterialInterface>> Originals;
		Originals.SetNum(NumSlots);
		for (int32 SlotIdx = 0; SlotIdx < NumSlots; ++SlotIdx)
		{
			Originals[SlotIdx] = Mesh->GetMaterial(SlotIdx);
			Mesh->SetMaterial(SlotIdx, DemolishHighlightMID);
		}
		CachedTargetMaterials.Add(Mesh, MoveTemp(Originals));
	}
}

void UHI_BuildManagerComponent::ClearHighlight(AHI_BuildableActor* Target)
{
	if (!Target) return;
	TArray<UMeshComponent*> Meshes;
	Target->GetComponents<UMeshComponent>(Meshes);
	for (UMeshComponent* Mesh : Meshes)
	{
		if (TArray<TObjectPtr<UMaterialInterface>>* Originals = CachedTargetMaterials.Find(Mesh))
		{
			for (int32 SlotIdx = 0; SlotIdx < Originals->Num(); ++SlotIdx)
			{
				Mesh->SetMaterial(SlotIdx, (*Originals)[SlotIdx]);
			}
			CachedTargetMaterials.Remove(Mesh);
		}
	}
}

UHI_BuildableDefinition* UHI_BuildManagerComponent::GetCurrentBuildable() const
{
	if (!AvailableBuildables.IsValidIndex(CurrentIndex)) return nullptr;
	return AvailableBuildables[CurrentIndex];
}

void UHI_BuildManagerComponent::ToggleBuildMode()
{
	SetBuildModeActive(!bActive);
}

void UHI_BuildManagerComponent::SetBuildModeActive(bool bNewActive)
{
	if (bActive == bNewActive) return;

	// Mutually exclusive with demolish mode.
	if (bNewActive && bDemolishActive)
	{
		bDemolishActive = false;
		UE_LOG(LogHadalIndustries, Log, TEXT("DemolishMode: OFF (build mode took over)"));
	}

	bActive = bNewActive;

	if (bActive)
	{
		if (AvailableBuildables.Num() == 0)
		{
			UE_LOG(LogHadalIndustries, Warning, TEXT("BuildManager: AvailableBuildables empty — cannot enter build mode"));
			bActive = false;
			return;
		}
		CurrentIndex = FMath::Clamp(CurrentIndex, 0, AvailableBuildables.Num() - 1);
		SpawnGhost();
		SetComponentTickEnabled(true);
		UE_LOG(LogHadalIndustries, Log, TEXT("BuildMode: ON (current = %s)"),
			GetCurrentBuildable() ? *GetCurrentBuildable()->BuildableId.ToString() : TEXT("<none>"));
	}
	else
	{
		DestroyGhost();
		SetComponentTickEnabled(false);
		UE_LOG(LogHadalIndustries, Log, TEXT("BuildMode: OFF"));
	}

	OnBuildModeChanged.Broadcast(bActive);
}

void UHI_BuildManagerComponent::CycleBuildable(int32 Direction)
{
	if (!bActive || AvailableBuildables.Num() <= 1 || Direction == 0) return;

	const int32 Step = Direction > 0 ? 1 : -1;
	CurrentIndex = (CurrentIndex + Step + AvailableBuildables.Num()) % AvailableBuildables.Num();

	UE_LOG(LogHadalIndustries, Log, TEXT("BuildMode: cycled to %s"),
		GetCurrentBuildable() ? *GetCurrentBuildable()->BuildableId.ToString() : TEXT("<none>"));

	// Rebuild the ghost with the new buildable's mesh.
	DestroyGhost();
	SpawnGhost();
	OnCurrentBuildableChanged.Broadcast(GetCurrentBuildable());
}

void UHI_BuildManagerComponent::RotatePreview(float Direction)
{
	if (!bActive || FMath::IsNearlyZero(Direction)) return;
	const float Step = (Direction > 0.f ? 1.f : -1.f) * RotationStepDegrees;
	CurrentYaw = FMath::Fmod(CurrentYaw + Step, 360.0f);
}

void UHI_BuildManagerComponent::CancelPlacement()
{
	SetBuildModeActive(false);
}

void UHI_BuildManagerComponent::ToggleDemolishMode()
{
	bDemolishActive = !bDemolishActive;
	if (bDemolishActive && bActive)
	{
		// Exit build mode while demolishing.
		SetBuildModeActive(false);
	}

	if (bDemolishActive)
	{
		SetComponentTickEnabled(true);
	}
	else
	{
		ClearHighlight(CurrentDemolishTarget);
		CurrentDemolishTarget = nullptr;
		if (!bActive) SetComponentTickEnabled(false);
	}

	UE_LOG(LogHadalIndustries, Log, TEXT("DemolishMode: %s"), bDemolishActive ? TEXT("ON (point + LMB to demolish)") : TEXT("OFF"));
}

void UHI_BuildManagerComponent::ToggleSnap()
{
	bSnapEnabled = !bSnapEnabled;
	UE_LOG(LogHadalIndustries, Log, TEXT("BuildMode: snap %s (grid %.0f cm)"),
		bSnapEnabled ? TEXT("ON") : TEXT("OFF"), GridSize);
}

bool UHI_BuildManagerComponent::DemolishTargeted()
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return false;

	UCameraComponent* Camera = OwnerPawn->FindComponentByClass<UCameraComponent>();
	const FVector Start = Camera ? Camera->GetComponentLocation() : OwnerPawn->GetActorLocation();
	const FVector Forward = Camera ? Camera->GetForwardVector() : OwnerPawn->GetActorForwardVector();
	const FVector End = Start + Forward * MaxPlacementDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(HI_DemolishTrace), false, OwnerPawn);
	Params.AddIgnoredActor(GhostActor);

	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params)) return false;

	AHI_BuildableActor* Target = Cast<AHI_BuildableActor>(Hit.GetActor());
	if (!Target) return false;

	// Refund.
	if (Target->SourceDefinition && DemolishRefundFraction > 0.f)
	{
		if (UHI_InventoryComponent* Inventory = GetOwnerInventory())
		{
			for (const FHI_ItemStack& Stack : Target->SourceDefinition->BuildCost)
			{
				const int32 RefundQty = FMath::Max(1, FMath::FloorToInt(Stack.Quantity * DemolishRefundFraction));
				Inventory->AddStack(Stack.ItemId, RefundQty);
			}
		}
	}

	UE_LOG(LogHadalIndustries, Log, TEXT("Demolished %s (refund %.0f%%)"),
		*Target->GetName(), DemolishRefundFraction * 100.f);
	Target->Destroy();
	return true;
}

bool UHI_BuildManagerComponent::TryConfirmPlace()
{
	if (!bActive)
	{
		return false;
	}

	UHI_BuildableDefinition* Def = GetCurrentBuildable();
	if (!Def || !Def->ActorClass)
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("BuildMode: no current buildable / ActorClass null"));
		return false;
	}

	if (!GhostActor)
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("BuildMode: no ghost to confirm"));
		return false;
	}

	const FTransform GhostXf = GhostActor->GetActorTransform();

	FString Reason;
	if (!ValidatePlacement(GhostXf, Reason))
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("BuildMode: cannot place %s — %s"),
			*Def->BuildableId.ToString(), *Reason);
		return false;
	}

	ConsumeMaterials(Def->BuildCost);

	UWorld* World = GetWorld();
	if (!World) return false;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Params.Owner = GetOwner();

	AHI_BuildableActor* Spawned = World->SpawnActor<AHI_BuildableActor>(
		Def->ActorClass, GhostXf.GetLocation(), GhostXf.Rotator(), Params);

	if (Spawned)
	{
		Spawned->BuildableId = Def->BuildableId;
		Spawned->SourceDefinition = Def;
		Spawned->SetActorScale3D(GhostXf.GetScale3D());
		UE_LOG(LogHadalIndustries, Log, TEXT("BuildMode: placed %s at %s"),
			*Def->BuildableId.ToString(), *GhostXf.GetLocation().ToCompactString());
	}
	else
	{
		UE_LOG(LogHadalIndustries, Error, TEXT("BuildMode: SpawnActor failed for %s"), *Def->BuildableId.ToString());
		return false;
	}

	return true;
}

void UHI_BuildManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bActive && GhostActor)
	{
		const bool bMoved = UpdateGhostTransform();
		if (bMoved)
		{
			FString Reason;
			const bool bValid = ValidatePlacement(GhostActor->GetActorTransform(), Reason);
			if (bValid != bLastValid)
			{
				UpdateGhostColor(bValid);
				bLastValid = bValid;
			}
		}
	}

	if (bDemolishActive)
	{
		UpdateDemolishHighlight();
	}
}

FVector UHI_BuildManagerComponent::ComputePlacementLocation() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return FVector::ZeroVector;

	UCameraComponent* Camera = OwnerPawn->FindComponentByClass<UCameraComponent>();
	const FVector Origin = Camera ? Camera->GetComponentLocation() : OwnerPawn->GetActorLocation();
	const FVector Forward = Camera ? Camera->GetForwardVector() : OwnerPawn->GetActorForwardVector();
	const FVector End = Origin + Forward * MaxPlacementDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(HI_BuildPlacementTrace), false, OwnerPawn);
	Params.AddIgnoredActor(GhostActor);

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Origin, End, ECC_Visibility, Params))
	{
		return Hit.ImpactPoint;
	}
	return End;
}

void UHI_BuildManagerComponent::SpawnGhost()
{
	UHI_BuildableDefinition* Def = GetCurrentBuildable();
	if (!Def) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.ObjectFlags |= RF_Transient;

	GhostActor = World->SpawnActor<AActor>(AActor::StaticClass(), FTransform::Identity, Params);
	if (!GhostActor) return;

	UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(GhostActor, TEXT("GhostMesh"));
	GhostActor->SetRootComponent(Mesh);
	Mesh->RegisterComponent();
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCastShadow(false);

	if (Def->PreviewMesh)
	{
		Mesh->SetStaticMesh(Def->PreviewMesh);
		Mesh->SetWorldScale3D(Def->PreviewScale);
	}

	GhostMID = nullptr;
	if (GhostMaterial && Mesh->GetStaticMesh())
	{
		GhostMID = UMaterialInstanceDynamic::Create(GhostMaterial, GhostActor);
		const int32 NumMats = Mesh->GetNumMaterials();
		for (int32 Index = 0; Index < NumMats; ++Index)
		{
			Mesh->SetMaterial(Index, GhostMID);
		}
	}

	bLastValid = false;
	UpdateGhostColor(false);
	UpdateGhostTransform();
}

void UHI_BuildManagerComponent::DestroyGhost()
{
	if (GhostActor)
	{
		GhostActor->Destroy();
		GhostActor = nullptr;
	}
	GhostMID = nullptr;
}

bool UHI_BuildManagerComponent::UpdateGhostTransform()
{
	if (!GhostActor) return false;

	FVector HitLoc = ComputePlacementLocation();

	// Snap-to-grid on X/Y (Z stays on the actual surface).
	if (bSnapEnabled && GridSize > 1.0f)
	{
		HitLoc.X = FMath::RoundToFloat(HitLoc.X / GridSize) * GridSize;
		HitLoc.Y = FMath::RoundToFloat(HitLoc.Y / GridSize) * GridSize;
	}

	const FRotator Rot(0.f, CurrentYaw, 0.f);
	const FQuat Q = Rot.Quaternion();

	// Default: actor sits at hit point.
	FVector ActorLoc = HitLoc;

	// Pivot-correct: place the mesh so its geometric center is over the hit point on X/Y
	// and its bottom rests on the hit point Z, regardless of where the pivot of the static mesh is
	// (engine BasicShapes have pivot at a corner).
	if (UStaticMeshComponent* Mesh = GhostActor->FindComponentByClass<UStaticMeshComponent>())
	{
		if (UStaticMesh* SM = Mesh->GetStaticMesh())
		{
			const FBox LocalBox = SM->GetBoundingBox();
			const FVector Scale = Mesh->GetRelativeScale3D();
			const FVector LocalCenter = LocalBox.GetCenter() * Scale;
			const FVector LocalExtent = LocalBox.GetExtent() * Scale;

			// Target world position for the geometric center: above the hit point by the half-height.
			const FVector TargetGeoCenter = HitLoc + FVector(0.f, 0.f, LocalExtent.Z);
			// Pivot offset from geometric center, rotated into world space.
			const FVector PivotOffset = Q.RotateVector(LocalCenter);
			ActorLoc = TargetGeoCenter - PivotOffset;
		}
	}

	GhostActor->SetActorLocationAndRotation(ActorLoc, Rot);
	return true;
}

void UHI_BuildManagerComponent::UpdateGhostColor(bool bValid)
{
	if (GhostMID)
	{
		GhostMID->SetVectorParameterValue(GhostColorParameter, bValid ? ValidColor : InvalidColor);
	}
}

bool UHI_BuildManagerComponent::ValidatePlacement(const FTransform& Xf, FString& OutReason) const
{
	UHI_BuildableDefinition* Def = GetCurrentBuildable();
	if (!Def)
	{
		OutReason = TEXT("no buildable selected");
		return false;
	}

	if (!HasMaterials(Def->BuildCost, OutReason))
	{
		return false;
	}

	// Collision sweep: probe full ghost mesh extent at target Xf.
	// We lift the box slightly so the ground (sitting tangent to the mesh bottom) is not picked up.
	if (GhostActor)
	{
		UStaticMeshComponent* Mesh = GhostActor->FindComponentByClass<UStaticMeshComponent>();
		if (Mesh && Mesh->GetStaticMesh())
		{
			const FVector ScaledCenter = Mesh->GetStaticMesh()->GetBoundingBox().GetCenter() * Mesh->GetRelativeScale3D();
			FVector Extent = Mesh->GetStaticMesh()->GetBoundingBox().GetExtent() * Mesh->GetRelativeScale3D();
			// Shrink XY by 1cm so two snapped buildables touching edge-to-edge are not considered overlapping.
			Extent.X = FMath::Max(Extent.X - 1.0f, 1.0f);
			Extent.Y = FMath::Max(Extent.Y - 1.0f, 1.0f);
			const FVector Center = Xf.GetLocation() + Xf.GetRotation().RotateVector(ScaledCenter) + FVector(0.f, 0.f, 2.0f);

			TArray<FOverlapResult> Overlaps;
			FCollisionQueryParams Params(SCENE_QUERY_STAT(HI_BuildOverlapCheck), false);
			Params.AddIgnoredActor(GhostActor);
			Params.AddIgnoredActor(GetOwner());

			FCollisionObjectQueryParams ObjParams;
			ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
			ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);

			GetWorld()->OverlapMultiByObjectType(
				Overlaps,
				Center,
				Xf.GetRotation(),
				ObjParams,
				FCollisionShape::MakeBox(Extent),
				Params);

			for (const FOverlapResult& Overlap : Overlaps)
			{
				if (AActor* Other = Overlap.GetActor())
				{
					if (Other->IsA(AHI_BuildableActor::StaticClass()))
					{
						OutReason = FString::Printf(TEXT("blocked by %s"), *Other->GetName());
						return false;
					}
				}
			}
		}
	}

	OutReason.Empty();
	return true;
}

bool UHI_BuildManagerComponent::HasMaterials(const TArray<FHI_ItemStack>& Cost, FString& OutReason) const
{
	if (Cost.Num() == 0) return true;

	UHI_InventoryComponent* Inventory = GetOwnerInventory();
	if (!Inventory)
	{
		OutReason = TEXT("no inventory on owner");
		return false;
	}

	for (const FHI_ItemStack& Stack : Cost)
	{
		if (!Inventory->HasItem(Stack.ItemId, Stack.Quantity))
		{
			OutReason = FString::Printf(TEXT("missing %s x%d (have %d)"),
				*Stack.ItemId.ToString(), Stack.Quantity, Inventory->GetTotalQuantity(Stack.ItemId));
			return false;
		}
	}
	return true;
}

void UHI_BuildManagerComponent::ConsumeMaterials(const TArray<FHI_ItemStack>& Cost)
{
	UHI_InventoryComponent* Inventory = GetOwnerInventory();
	if (!Inventory) return;
	for (const FHI_ItemStack& Stack : Cost)
	{
		Inventory->RemoveItem(Stack.ItemId, Stack.Quantity);
	}
}

UHI_InventoryComponent* UHI_BuildManagerComponent::GetOwnerInventory() const
{
	if (!GetOwner()) return nullptr;
	return GetOwner()->FindComponentByClass<UHI_InventoryComponent>();
}
