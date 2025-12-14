#include "SettingsWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GraphicsSettingsWidget.h"
#include "FibulaCharacter.h"
#include "FloatingTextWidget.h"
#include "FibulaHUD.h"
#include "SoundSettingsWidget.h"

USettingsWidget::USettingsWidget(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<UGraphicsSettingsWidget> GraphicsSettingsWidgetBPClass(TEXT("/Game/HUDs/Widgets/WBP_GraphicsSettingsWidget"));
    if (GraphicsSettingsWidgetBPClass.Class != nullptr)
    {
        GraphicsSettingsWidgetClass = GraphicsSettingsWidgetBPClass.Class;
    }

    static ConstructorHelpers::FClassFinder<USoundSettingsWidget> SoundSettingsWidgetBPClass(TEXT("/Game/HUDs/Widgets/WBP_SoundSettingsWidget"));
    if (SoundSettingsWidgetBPClass.Class != nullptr)
    {
        SoundSettingsWidgetClass = SoundSettingsWidgetBPClass.Class;
    }
}

void USettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    
    if (LogoutButton)
    {
        LogoutButton->OnClicked.RemoveAll(this);
        LogoutButton->OnClicked.AddDynamic(this, &USettingsWidget::OnLogoutButtonClicked);
    }

    if (CloseButton)
    {
        CloseButton->OnClicked.RemoveAll(this);
        CloseButton->OnClicked.AddDynamic(this, &USettingsWidget::OnCloseButtonClicked);
    }

    if (GraphicsButton)
    {
        GraphicsButton->OnClicked.RemoveAll(this);
        GraphicsButton->OnClicked.AddDynamic(this, &USettingsWidget::OnGraphicsButtonClicked);
    }

    if (SoundButton)
    {
        SoundButton->OnClicked.RemoveAll(this);
        SoundButton->OnClicked.AddDynamic(this, &USettingsWidget::OnSoundButtonClicked);
    }

    if (CancelLogoutButton)
    {
        CancelLogoutButton->OnClicked.RemoveAll(this);
        CancelLogoutButton->OnClicked.AddDynamic(this, &USettingsWidget::OnCancelLogoutButtonClicked);
        CancelLogoutButton->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (LogoutTimerText)
    {
        LogoutTimerText->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void USettingsWidget::NativeDestruct()
{
    
    if (LogoutButton)
        LogoutButton->OnClicked.RemoveAll(this);

    if (CloseButton)
        CloseButton->OnClicked.RemoveAll(this);

    if (GraphicsButton)
        GraphicsButton->OnClicked.RemoveAll(this);

    if (SoundButton)
        SoundButton->OnClicked.RemoveAll(this);

    CancelLogout();

    Super::NativeDestruct();
}

void USettingsWidget::OnLogoutButtonClicked()
{
    if (APlayerController *PC = GetOwningPlayer())
    {
        if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(PC->GetPawn()))
        {
            if (Character->IsInCombat())
            {
                InitiateLogout();
                return;
            }

            
            PC->ConsoleCommand("Disconnect");
        }
    }
}

void USettingsWidget::InitiateLogout()
{
    if (APlayerController *PC = GetOwningPlayer())
    {
        if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(PC->GetPawn()))
        {
            LogoutTimeRemaining = 30.0f;
            ShowErrorMessage(TEXT("You are in combat! You will be logged out in 30 seconds. Your character will remain online until then."));

            if (LogoutTimerText)
            {
                LogoutTimerText->SetVisibility(ESlateVisibility::Visible);
                UpdateLogoutTimer();
            }

            if (CancelLogoutButton)
            {
                CancelLogoutButton->SetVisibility(ESlateVisibility::Visible);
            }

            GetWorld()->GetTimerManager().SetTimer(
                LogoutTimerHandle,
                this,
                &USettingsWidget::UpdateLogoutTimer,
                1.0f,
                true);
        }
    }
}

void USettingsWidget::UpdateLogoutTimer()
{
    LogoutTimeRemaining -= 1.0f;

    if (LogoutTimerText)
    {
        LogoutTimerText->SetText(FText::FromString(FString::Printf(TEXT("Logout in: %.0f seconds"), LogoutTimeRemaining)));
    }

    if (LogoutTimeRemaining <= 0.0f)
    {
        if (APlayerController *PC = GetOwningPlayer())
        {
            PC->ConsoleCommand("Disconnect");
            PC->ClientTravel("/Game/Maps/LoginScreen", TRAVEL_Absolute);
        }
        CancelLogout();
    }
}

void USettingsWidget::OnCancelLogoutButtonClicked()
{
    CancelLogout();
    ShowErrorMessage(TEXT("Logout cancelled."));
}

void USettingsWidget::CancelLogout()
{
    GetWorld()->GetTimerManager().ClearTimer(LogoutTimerHandle);
    if (LogoutTimerText)
    {
        LogoutTimerText->SetVisibility(ESlateVisibility::Collapsed);
    }
    if (CancelLogoutButton)
    {
        CancelLogoutButton->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void USettingsWidget::ShowErrorMessage(const FString &Message)
{
    if (APlayerController *PC = GetOwningPlayer())
    {
        if (AFibulaCharacter *Character = Cast<AFibulaCharacter>(PC->GetPawn()))
        {
            Character->ShowCenteredMessage(Message, 3.0f, EFloatingTextType::MinorInfo);
        }
    }
}

void USettingsWidget::OnCloseButtonClicked()
{
    RemoveFromParent();
}

void USettingsWidget::OnGraphicsButtonClicked()
{
    if (!GraphicsSettingsWidget && GraphicsSettingsWidgetClass)
    {
        GraphicsSettingsWidget = CreateWidget<UGraphicsSettingsWidget>(GetWorld(), GraphicsSettingsWidgetClass);
    }

    if (GraphicsSettingsWidget)
    {
        GraphicsSettingsWidget->AddToViewport();
    }
}

void USettingsWidget::OnSoundButtonClicked()
{
    if (!SoundSettingsWidget && SoundSettingsWidgetClass)
    {
        SoundSettingsWidget = CreateWidget<USoundSettingsWidget>(GetWorld(), SoundSettingsWidgetClass);
    }

    if (SoundSettingsWidget)
    {
        SoundSettingsWidget->AddToViewport();
    }
}