#include "FibulaLoginGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "LoginWidget.h"
#include "FibulaPlayerState.h"

AFibulaLoginGameMode::AFibulaLoginGameMode()
{
    PrimaryActorTick.bCanEverTick = false;
    bUseSeamlessTravel = false;

    
    static ConstructorHelpers::FClassFinder<ULoginWidget> LoginWidgetBPClass(TEXT("/Game/HUDs/Widgets/WBP_LoginWidget"));
    if (LoginWidgetBPClass.Class != nullptr)
    {
        LoginWidgetClass = LoginWidgetBPClass.Class;
    }
    PlayerStateClass = AFibulaPlayerState::StaticClass();
}

void AFibulaLoginGameMode::InitGame(const FString &MapName, const FString &Options, FString &ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
}

void AFibulaLoginGameMode::BeginPlay()
{
    Super::BeginPlay();

    APlayerController *PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC)
    {
        return;
    }

    if (!LoginWidgetClass)
    {
        return;
    }

    ULoginWidget *LoginWidget = CreateWidget<ULoginWidget>(PC, LoginWidgetClass);
    if (!LoginWidget)
    {
        return;
    }

    LoginWidget->AddToViewport();

    
    FInputModeUIOnly InputMode;
    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;
}
