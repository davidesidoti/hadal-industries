#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HI_PlayerCharacter.generated.h"

class UCameraComponent;
class USkeletalMeshComponent;
class UInputAction;
class UInputMappingContext;
class UHI_InteractionComponent;
class UHI_ScannerComponent;
struct FInputActionValue;

UCLASS()
class HADALINDUSTRIES_API AHI_PlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hadal|Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hadal|Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hadal|Components", meta = (AllowPrivateAccess = "true"))
	UHI_InteractionComponent* InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hadal|Components", meta = (AllowPrivateAccess = "true"))
	UHI_ScannerComponent* ScannerComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Input")
	TArray<UInputMappingContext*> InputMappingContexts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Input")
	UInputAction* VerticalAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Input")
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hadal|Input")
	UInputAction* ScanAction;

public:
	AHI_PlayerCharacter();

	virtual void PawnClientRestart() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Hadal|Components")
	UHI_InteractionComponent* GetInteractionComponent() const { return InteractionComponent; }

	UFUNCTION(BlueprintPure, Category = "Hadal|Components")
	UHI_ScannerComponent* GetScannerComponent() const { return ScannerComponent; }

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleVertical(const FInputActionValue& Value);
	void HandleJumpStart();
	void HandleJumpEnd();
	void HandleInteract();
	void HandleScan();

private:
	bool bIsAscending = false;
};
