#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SoundSettingsManager.generated.h"

UCLASS()
class FIBULA_API USoundSettingsManager : public USaveGame
{
    GENERATED_BODY()

public:
    USoundSettingsManager();

    UPROPERTY(SaveGame)
    float MasterVolume;

    UPROPERTY(SaveGame)
    float BackgroundMusicVolume;

    UPROPERTY(SaveGame)
    float SoundEffectsVolume;

    static USoundSettingsManager *LoadSettings();
    void SaveSettings();
    static FString GetSaveSlotName() { return TEXT("SoundSettings"); }
};