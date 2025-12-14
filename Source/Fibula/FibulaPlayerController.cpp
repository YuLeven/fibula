#include "FibulaPlayerController.h"
#include "FibulaCharacter.h"
#include "TimerManager.h"
#include "FibulaGameMode.h"
#include "Framework/Application/NavigationConfig.h"

AFibulaPlayerController::AFibulaPlayerController()
{
    bAutoManageActiveCameraTarget = true;
}

void AFibulaPlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (IsLocalPlayerController())
    {
        TSharedRef<FNavigationConfig> Navigation = MakeShared<FNavigationConfig>();
        Navigation->bKeyNavigation = false;
        Navigation->bTabNavigation = false;
        Navigation->bAnalogNavigation = false;
        FSlateApplication::Get().SetNavigationConfig(Navigation);
    }
}

void AFibulaPlayerController::PawnLeavingGame()
{
    if (AFibulaCharacter *PlayerCharacter = Cast<AFibulaCharacter>(GetPawn()))
    {
        if (HasAuthority() && PlayerCharacter->IsInCombat())
        {
            
            PlayerCharacter->HandleCombatLogout();

            
            UnPossess();
        }
        else
        {
            Super::PawnLeavingGame();
        }
    }
    else
    {
        Super::PawnLeavingGame();
    }
}