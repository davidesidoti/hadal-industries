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

#include "Core/HI_Log.h"
#include "Interaction/HI_InteractionComponent.h"
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
