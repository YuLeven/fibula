#include "SoundSettingsWidget.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "BackgroundMusicManager.h"

void USoundSettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ApplyButton)
    {
        ApplyButton->OnClicked.RemoveAll(this);
        ApplyButton->OnClicked.AddDynamic(this, &USoundSettingsWidget::OnApplyButtonClicked);
    }

    if (CloseButton)
    {
        CloseButton->OnClicked.RemoveAll(this);
        CloseButton->OnClicked.AddDynamic(this, &USoundSettingsWidget::OnCloseButtonClicked);
    }

    LoadCurrentSettings();
}

void USoundSettingsWidget::LoadCurrentSettings()
{
    SettingsManager = USoundSettingsManager::LoadSettings();

    if (MasterVolumeSlider)
    {
        MasterVolumeSlider->SetValue(SettingsManager->MasterVolume);
    }

    if (BackgroundMusicVolumeSlider)
    {
        BackgroundMusicVolumeSlider->SetValue(SettingsManager->BackgroundMusicVolume);
    }

    if (SoundEffectsVolumeSlider)
    {
        SoundEffectsVolumeSlider->SetValue(SettingsManager->SoundEffectsVolume);
    }
}

void USoundSettingsWidget::ApplySettings()
{
    if (!SettingsManager)
    {
        SettingsManager = USoundSettingsManager::LoadSettings();
    }

    if (MasterVolumeSlider)
    {
        SettingsManager->MasterVolume = MasterVolumeSlider->GetValue();
    }

    if (BackgroundMusicVolumeSlider)
    {
        SettingsManager->BackgroundMusicVolume = BackgroundMusicVolumeSlider->GetValue();
    }

    if (SoundEffectsVolumeSlider)
    {
        SettingsManager->SoundEffectsVolume = SoundEffectsVolumeSlider->GetValue();
    }

    SettingsManager->SaveSettings();

    
    if (UGameplayStatics::GetGameInstance(this))
    {
        UGameplayStatics::SetSoundMixClassOverride(this, nullptr, nullptr, SettingsManager->MasterVolume, 1.0f, 0.0f);

        
        if (ABackgroundMusicManager *MusicManager = Cast<ABackgroundMusicManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABackgroundMusicManager::StaticClass())))
        {
            MusicManager->UpdateCurrentMusicVolume(SettingsManager->MasterVolume, SettingsManager->BackgroundMusicVolume);
        }
    }
}

void USoundSettingsWidget::OnApplyButtonClicked()
{
    ApplySettings();
}

void USoundSettingsWidget::OnCloseButtonClicked()
{
    RemoveFromParent();
}