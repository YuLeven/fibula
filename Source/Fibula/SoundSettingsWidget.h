#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SoundSettingsManager.h"
#include "SoundSettingsWidget.generated.h"

class USlider;
class UButton;

UCLASS()
class FIBULA_API USoundSettingsWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    USlider *MasterVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    USlider *BackgroundMusicVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    USlider *SoundEffectsVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    UButton *ApplyButton;

    UPROPERTY(meta = (BindWidget))
    UButton *CloseButton;

    UFUNCTION()
    void OnApplyButtonClicked();

    UFUNCTION()
    void OnCloseButtonClicked();

private:
    UPROPERTY()
    USoundSettingsManager *SettingsManager;

    void LoadCurrentSettings();
    void ApplySettings();
};