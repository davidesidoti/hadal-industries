#include "Core/HI_GameMode.h"
#include "Core/HI_PlayerCharacter.h"
#include "Core/HI_PlayerController.h"
#include "Core/HI_Log.h"

AHI_GameMode::AHI_GameMode()
{
	DefaultPawnClass = AHI_PlayerCharacter::StaticClass();
	PlayerControllerClass = AHI_PlayerController::StaticClass();

	UE_LOG(LogHadalIndustries, Log, TEXT("AHI_GameMode constructed (M0 foundation)"));
}
