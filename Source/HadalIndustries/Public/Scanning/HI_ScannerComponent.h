#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HI_ScannerComponent.generated.h"

class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHI_ScanPulseFinished, const TArray<AActor*>&, DetectedActors);

UCLASS(ClassGroup = ("Hadal"), meta = (BlueprintSpawnableComponent))
class HADALINDUSTRIES_API UHI_ScannerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHI_ScannerComponent();

	UFUNCTION(BlueprintCallable, Category = "Hadal|Scanning")
	void Pulse();

	UPROPERTY(BlueprintAssignable, Category = "Hadal|Scanning")
	FHI_ScanPulseFinished OnScanPulseFinished;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Scanning", meta = (ClampMin = "10.0"))
	float PulseRadius = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Scanning")
	float CooldownSeconds = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Scanning")
	bool bDrawDebugSphere = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hadal|Scanning")
	TArray<TSubclassOf<AActor>> DetectableClasses;

private:
	float LastPulseTime = -1.0e9f;
};
