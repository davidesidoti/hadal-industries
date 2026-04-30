#include "Scanning/HI_ScannerComponent.h"

#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#include "Core/HI_Log.h"

UHI_ScannerComponent::UHI_ScannerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHI_ScannerComponent::Pulse()
{
	UWorld* World = GetWorld();
	if (!World) return;

	const float Now = World->GetTimeSeconds();
	if (Now - LastPulseTime < CooldownSeconds)
	{
		UE_LOG(LogHadalIndustries, Verbose, TEXT("Scan pulse on cooldown"));
		return;
	}
	LastPulseTime = Now;

	const FVector Origin = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;

	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(PulseRadius);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(HI_ScanPulse), false, GetOwner());

	World->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		FQuat::Identity,
		FCollisionObjectQueryParams::AllObjects,
		Sphere,
		Params);

	TArray<AActor*> Detected;
	Detected.Reserve(Overlaps.Num());

	const bool bFilterByClass = DetectableClasses.Num() > 0;

	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* Actor = Result.GetActor();
		if (!Actor || Actor == GetOwner()) continue;

		if (bFilterByClass)
		{
			bool bMatches = false;
			for (const TSubclassOf<AActor>& Class : DetectableClasses)
			{
				if (Class && Actor->IsA(Class)) { bMatches = true; break; }
			}
			if (!bMatches) continue;
		}

		Detected.AddUnique(Actor);
	}

	UE_LOG(LogHadalIndustries, Log, TEXT("Scan pulse @ %s — radius %.0f — detected %d actor(s)"),
		*Origin.ToCompactString(), PulseRadius, Detected.Num());

#if ENABLE_DRAW_DEBUG
	if (bDrawDebugSphere)
	{
		DrawDebugSphere(World, Origin, PulseRadius, 24, FColor::Cyan, false, 1.0f, 0, 1.0f);
		for (AActor* Actor : Detected)
		{
			DrawDebugLine(World, Origin, Actor->GetActorLocation(), FColor::Yellow, false, 1.0f, 0, 1.5f);
		}
	}
#endif

	OnScanPulseFinished.Broadcast(Detected);
}
