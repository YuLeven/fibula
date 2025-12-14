#include "GraphicsSettingsWidget.h"
#include "Components/ComboBoxString.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/GameViewportClient.h"

void UGraphicsSettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ApplyButton)
    {
        ApplyButton->OnClicked.AddDynamic(this, &UGraphicsSettingsWidget::OnApplyButtonClicked);
    }

    if (CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &UGraphicsSettingsWidget::OnCloseButtonClicked);
    }

    PopulateResolutionOptions();
    PopulateQualityOptions();
    InitializeVSyncOption();
}

void UGraphicsSettingsWidget::PopulateResolutionOptions()
{
    if (!ResolutionComboBox)
        return;

    ResolutionComboBox->ClearOptions();

    
    TArray<FIntPoint> CommonResolutions = {
        FIntPoint(1280, 720),  
        FIntPoint(1920, 1080), 
        FIntPoint(2560, 1440), 
        FIntPoint(3840, 2160)  
    };

    
    TArray<FIntPoint> UltrawideResolutions = {
        FIntPoint(2560, 1080), 
        FIntPoint(3440, 1440), 
    };

    
    TArray<FIntPoint> AllResolutions;
    AllResolutions.Append(CommonResolutions);
    AllResolutions.Append(UltrawideResolutions);

    
    AllResolutions.Sort([](const FIntPoint &A, const FIntPoint &B)
                        {
        if (A.X != B.X)
            return A.X > B.X;
        return A.Y > B.Y; });

    
    for (const FIntPoint &Resolution : AllResolutions)
    {
        FString ResolutionStr = FString::Printf(TEXT("%dx%d"), Resolution.X, Resolution.Y);
        ResolutionComboBox->AddOption(ResolutionStr);
    }

    
    UGameUserSettings *Settings = UGameUserSettings::GetGameUserSettings();
    FIntPoint CurrentRes = Settings->GetScreenResolution();
    FString CurrentResString = FString::Printf(TEXT("%dx%d"), CurrentRes.X, CurrentRes.Y);
    ResolutionComboBox->SetSelectedOption(CurrentResString);
}

void UGraphicsSettingsWidget::PopulateQualityOptions()
{
    if (!QualityComboBox)
        return;

    QualityComboBox->ClearOptions();
    QualityComboBox->AddOption(TEXT("Low"));
    QualityComboBox->AddOption(TEXT("Medium"));
    QualityComboBox->AddOption(TEXT("High"));
    QualityComboBox->AddOption(TEXT("Epic"));

    UGameUserSettings *Settings = UGameUserSettings::GetGameUserSettings();
    int32 Quality = Settings->GetOverallScalabilityLevel();
    QualityComboBox->SetSelectedIndex(Quality);
}

void UGraphicsSettingsWidget::ApplySettings()
{
    UGameUserSettings *Settings = UGameUserSettings::GetGameUserSettings();

    
    FString SelectedRes = ResolutionComboBox->GetSelectedOption();
    FString X, Y;
    SelectedRes.Split(TEXT("x"), &X, &Y);
    FIntPoint NewResolution(FCString::Atoi(*X), FCString::Atoi(*Y));
    Settings->SetScreenResolution(NewResolution);

    
    int32 QualityLevel = QualityComboBox->GetSelectedIndex();
    Settings->SetOverallScalabilityLevel(QualityLevel);

    
    if (VSyncCheckBox)
    {
        bool bVSync = VSyncCheckBox->IsChecked();
        Settings->SetVSyncEnabled(bVSync);

        
        if (GEngine)
        {
            if (UGameViewportClient *Viewport = GEngine->GameViewport)
            {
                static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSync"));
                if (CVar)
                {
                    CVar->Set(bVSync ? 1 : 0, ECVF_SetByGameSetting);
                }
            }
        }
    }

    
    Settings->ApplySettings(false);
    Settings->SaveSettings();
}

void UGraphicsSettingsWidget::OnApplyButtonClicked()
{
    ApplySettings();
}

void UGraphicsSettingsWidget::OnCloseButtonClicked()
{
    RemoveFromParent();
}

void UGraphicsSettingsWidget::InitializeVSyncOption()
{
    if (!VSyncCheckBox)
        return;

    UGameUserSettings *Settings = UGameUserSettings::GetGameUserSettings();
    VSyncCheckBox->SetIsChecked(Settings->IsVSyncEnabled());
}