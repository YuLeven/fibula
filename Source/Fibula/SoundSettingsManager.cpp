#include "SoundSettingsManager.h"
#include "Kismet/GameplayStatics.h"

USoundSettingsManager::USoundSettingsManager()
{
    MasterVolume = 1.0f;
    BackgroundMusicVolume = 1.0f;
    SoundEffectsVolume = 1.0f;
}

USoundSettingsManager *USoundSettingsManager::LoadSettings()
{
    USoundSettingsManager *Settings = Cast<USoundSettingsManager>(UGameplayStatics::LoadGameFromSlot(GetSaveSlotName(), 0));
    if (!Settings)
    {
        Settings = Cast<USoundSettingsManager>(UGameplayStatics::CreateSaveGameObject(USoundSettingsManager::StaticClass()));
    }
    return Settings;
}

void USoundSettingsManager::SaveSettings()
{
    UGameplayStatics::SaveGameToSlot(this, GetSaveSlotName(), 0);
}