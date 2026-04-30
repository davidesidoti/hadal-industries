#include "Interaction/HI_InteractionComponent.h"

#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"

#include "Core/HI_Log.h"
#include "Interaction/HI_InteractableInterface.h"

UHI_InteractionComponent::UHI_InteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UHI_InteractionComponent::TraceForward(FHitResult& OutHit) const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return false;

	UCameraComponent* Camera = OwnerPawn->FindComponentByClass<UCameraComponent>();
	if (!Camera) return false;

	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * TraceDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(HI_InteractionTrace), false, OwnerPawn);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);

#if ENABLE_DRAW_DEBUG
	if (bDrawDebugTrace)
	{
		const FColor Color = bHit ? FColor::Green : FColor::Red;
		DrawDebugLine(GetWorld(), Start, End, Color, false, 0.5f, 0, 1.0f);
	}
#endif

	return bHit;
}

AActor* UHI_InteractionComponent::QueryFocusedInteractable() const
{
	FHitResult Hit;
	if (!TraceForward(Hit)) return nullptr;

	AActor* HitActor = Hit.GetActor();
	if (!HitActor || !HitActor->Implements<UHI_InteractableInterface>()) return nullptr;

	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!IHI_InteractableInterface::Execute_CanBeInteractedWith(HitActor, OwnerPawn)) return nullptr;

	return HitActor;
}

bool UHI_InteractionComponent::TryInteract()
{
	AActor* Target = QueryFocusedInteractable();
	if (!Target)
	{
		UE_LOG(LogHadalIndustries, Verbose, TEXT("Interact: no focused interactable"));
		return false;
	}

	UE_LOG(LogHadalIndustries, Log, TEXT("Interact: %s"), *Target->GetName());
	IHI_InteractableInterface::Execute_OnInteract(Target, GetOwner());
	return true;
}
