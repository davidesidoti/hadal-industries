#include "Core/HI_PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

#include "BuildSystem/HI_BuildManagerComponent.h"
#include "Core/HI_Log.h"
#include "Interaction/HI_InteractionComponent.h"
#include "Inventory/HI_InventoryComponent.h"
#include "Machines/HI_MachineBase.h"
#include "Scanning/HI_ScannerComponent.h"

AHI_PlayerCharacter::AHI_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(34.0f, 96.0f);

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	UCharacterMovementComponent* CMC = GetCharacterMovement();
	CMC->BrakingDecelerationFalling = 1500.0f;
	CMC->AirControl = 0.5f;
	CMC->MaxSwimSpeed = 600.0f;
	CMC->BrakingDecelerationSwimming = 2048.0f;
	CMC->Buoyancy = 1.0f;
	CMC->bCanWalkOffLedgesWhenCrouching = true;

	InteractionComponent = CreateDefaultSubobject<UHI_InteractionComponent>(TEXT("InteractionComponent"));
	ScannerComponent = CreateDefaultSubobject<UHI_ScannerComponent>(TEXT("ScannerComponent"));
	InventoryComponent = CreateDefaultSubobject<UHI_InventoryComponent>(TEXT("InventoryComponent"));
	BuildManagerComponent = CreateDefaultSubobject<UHI_BuildManagerComponent>(TEXT("BuildManagerComponent"));
}

void AHI_PlayerCharacter::GrantItem(FName ItemId, int32 Quantity)
{
	if (!InventoryComponent)
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("GrantItem: no InventoryComponent"));
		return;
	}
	const int32 Leftover = InventoryComponent->AddStack(ItemId, Quantity);
	UE_LOG(LogHadalIndustries, Log, TEXT("GrantItem: %s x%d (leftover %d)"),
		*ItemId.ToString(), Quantity, Leftover);
	InventoryComponent->DebugLogContents();
}

void AHI_PlayerCharacter::DumpInventory()
{
	if (InventoryComponent)
	{
		InventoryComponent->DebugLogContents();
	}
}

void AHI_PlayerCharacter::GrantToTargetMachine(FName ItemId, int32 Quantity)
{
	if (Quantity <= 0 || ItemId.IsNone())
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("GrantToTargetMachine: invalid args (%s x%d)"), *ItemId.ToString(), Quantity);
		return;
	}

	const FVector Start = FirstPersonCameraComponent ? FirstPersonCameraComponent->GetComponentLocation() : GetActorLocation();
	const FVector Forward = FirstPersonCameraComponent ? FirstPersonCameraComponent->GetForwardVector() : GetActorForwardVector();
	const FVector End = Start + Forward * 600.0f;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(HI_GrantTargetTrace), false, this);
	FHitResult Hit;
	if (!GetWorld() || !GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("GrantToTargetMachine: no hit forward"));
		return;
	}

	AHI_MachineBase* Machine = Cast<AHI_MachineBase>(Hit.GetActor());
	if (!Machine)
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("GrantToTargetMachine: hit %s is not an AHI_MachineBase"),
			Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("<null>"));
		return;
	}

	UHI_InventoryComponent* Input = Machine->InputInventory;
	if (!Input)
	{
		UE_LOG(LogHadalIndustries, Warning, TEXT("GrantToTargetMachine: machine %s has no InputInventory"), *Machine->GetName());
		return;
	}

	const int32 Leftover = Input->AddStack(ItemId, Quantity);
	UE_LOG(LogHadalIndustries, Log, TEXT("GrantToTargetMachine: %s x%d -> %s (leftover %d)"),
		*ItemId.ToString(), Quantity, *Machine->GetName(), Leftover);
	Input->DebugLogContents();
}

void AHI_PlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			if (InputMappingContexts.Num() == 0)
			{
				UE_LOG(LogHadalIndustries, Warning, TEXT("AHI_PlayerCharacter '%s': InputMappingContexts is empty"), *GetNameSafe(this));
			}
			for (UInputMappingContext* Context : InputMappingContexts)
			{
				if (Context) Subsystem->AddMappingContext(Context, 0);
			}
		}
	}
}

void AHI_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)        EIC->BindAction(MoveAction,        ETriggerEvent::Triggered, this, &AHI_PlayerCharacter::HandleMove);
		if (LookAction)        EIC->BindAction(LookAction,        ETriggerEvent::Triggered, this, &AHI_PlayerCharacter::HandleLook);
		if (MouseLookAction)   EIC->BindAction(MouseLookAction,   ETriggerEvent::Triggered, this, &AHI_PlayerCharacter::HandleLook);
		if (VerticalAction)    EIC->BindAction(VerticalAction,    ETriggerEvent::Triggered, this, &AHI_PlayerCharacter::HandleVertical);
		if (JumpAction)
		{
			EIC->BindAction(JumpAction, ETriggerEvent::Started,   this, &AHI_PlayerCharacter::HandleJumpStart);
			EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &AHI_PlayerCharacter::HandleJumpEnd);
		}
		if (InteractAction)    EIC->BindAction(InteractAction,    ETriggerEvent::Started,   this, &AHI_PlayerCharacter::HandleInteract);
		if (ScanAction)        EIC->BindAction(ScanAction,        ETriggerEvent::Started,   this, &AHI_PlayerCharacter::HandleScan);

		if (BuildToggleAction)    EIC->BindAction(BuildToggleAction,    ETriggerEvent::Started,   this, &AHI_PlayerCharacter::HandleBuildToggle);
		if (RotatePreviewAction)  EIC->BindAction(RotatePreviewAction,  ETriggerEvent::Started,   this, &AHI_PlayerCharacter::HandleRotatePreview);
		if (CycleBuildableAction) EIC->BindAction(CycleBuildableAction, ETriggerEvent::Triggered, this, &AHI_PlayerCharacter::HandleCycleBuildable);
		if (ConfirmPlaceAction)   EIC->BindAction(ConfirmPlaceAction,   ETriggerEvent::Started,   this, &AHI_PlayerCharacter::HandleConfirmPlace);
		if (CancelPlaceAction)    EIC->BindAction(CancelPlaceAction,    ETriggerEvent::Started,   this, &AHI_PlayerCharacter::HandleCancelPlace);
		if (ToggleSnapAction)     EIC->BindAction(ToggleSnapAction,     ETriggerEvent::Started,   this, &AHI_PlayerCharacter::HandleToggleSnap);
		if (DemolishAction)       EIC->BindAction(DemolishAction,       ETriggerEvent::Started,   this, &AHI_PlayerCharacter::HandleDemolish);
	}
	else
	{
		UE_LOG(LogHadalIndustries, Error, TEXT("AHI_PlayerCharacter '%s': missing EnhancedInputComponent"), *GetNameSafe(this));
	}
}

void AHI_PlayerCharacter::HandleMove(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (!GetController()) return;

	AddMovementInput(GetActorForwardVector(), Axis.Y);
	AddMovementInput(GetActorRightVector(),   Axis.X);
}

void AHI_PlayerCharacter::HandleLook(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (!GetController()) return;

	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void AHI_PlayerCharacter::HandleVertical(const FInputActionValue& Value)
{
	const float Axis = Value.Get<float>();
	if (FMath::IsNearlyZero(Axis) || !GetController()) return;

	if (GetCharacterMovement()->IsSwimming())
	{
		AddMovementInput(FVector::UpVector, Axis);
	}
}

void AHI_PlayerCharacter::HandleJumpStart()
{
	bIsAscending = true;
	Jump();
}

void AHI_PlayerCharacter::HandleJumpEnd()
{
	bIsAscending = false;
	StopJumping();
}

void AHI_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsAscending && GetController() && GetCharacterMovement()->IsSwimming())
	{
		AddMovementInput(FVector::UpVector, 1.0f);
	}
}

void AHI_PlayerCharacter::HandleInteract()
{
	if (InteractionComponent)
	{
		InteractionComponent->TryInteract();
	}
}

void AHI_PlayerCharacter::HandleScan()
{
	if (ScannerComponent)
	{
		ScannerComponent->Pulse();
	}
}

void AHI_PlayerCharacter::HandleBuildToggle()
{
	if (BuildManagerComponent)
	{
		BuildManagerComponent->ToggleBuildMode();
	}
}

void AHI_PlayerCharacter::HandleRotatePreview()
{
	if (BuildManagerComponent)
	{
		BuildManagerComponent->RotatePreview(1.0f);
	}
}

void AHI_PlayerCharacter::HandleCycleBuildable(const FInputActionValue& Value)
{
	if (!BuildManagerComponent) return;
	const float Axis = Value.Get<float>();
	if (FMath::IsNearlyZero(Axis)) return;
	BuildManagerComponent->CycleBuildable(Axis > 0.f ? +1 : -1);
}

void AHI_PlayerCharacter::HandleConfirmPlace()
{
	if (!BuildManagerComponent) return;
	if (BuildManagerComponent->IsBuildModeActive())
	{
		BuildManagerComponent->TryConfirmPlace();
	}
	else if (BuildManagerComponent->IsDemolishModeActive())
	{
		BuildManagerComponent->DemolishTargeted();
	}
}

void AHI_PlayerCharacter::HandleCancelPlace()
{
	if (!BuildManagerComponent) return;
	if (BuildManagerComponent->IsBuildModeActive())
	{
		BuildManagerComponent->CancelPlacement();
	}
	else if (BuildManagerComponent->IsDemolishModeActive())
	{
		BuildManagerComponent->ToggleDemolishMode();
	}
}

void AHI_PlayerCharacter::HandleToggleSnap()
{
	if (BuildManagerComponent)
	{
		BuildManagerComponent->ToggleSnap();
	}
}

void AHI_PlayerCharacter::HandleDemolish()
{
	if (BuildManagerComponent)
	{
		BuildManagerComponent->ToggleDemolishMode();
	}
}
