#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsWidget.generated.h"

class UButton;
class UGraphicsSettingsWidget;
class USoundSettingsWidget;

UCLASS()
class FIBULA_API USettingsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    USettingsWidget(const FObjectInitializer &ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    UButton *LogoutButton;

    UPROPERTY(meta = (BindWidget))
    UButton *CloseButton;

    UPROPERTY(meta = (BindWidget))
    UButton *GraphicsButton;

    UPROPERTY(meta = (BindWidget))
    UButton *SoundButton;

    UPROPERTY(meta = (BindWidget))
    UButton *CancelLogoutButton;

    UPROPERTY()
    UGraphicsSettingsWidget *GraphicsSettingsWidget;

    UPROPERTY()
    USoundSettingsWidget *SoundSettingsWidget;

    UPROPERTY()
    TSubclassOf<UGraphicsSettingsWidget> GraphicsSettingsWidgetClass;

    UPROPERTY()
    TSubclassOf<USoundSettingsWidget> SoundSettingsWidgetClass;

    UPROPERTY()
    FTimerHandle LogoutTimerHandle;

    UPROPERTY()
    float LogoutTimeRemaining;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock *LogoutTimerText;

    UFUNCTION()
    void OnLogoutButtonClicked();

    UFUNCTION()
    void OnCloseButtonClicked();

    UFUNCTION()
    void OnGraphicsButtonClicked();

    UFUNCTION()
    void OnSoundButtonClicked();

    UFUNCTION()
    void UpdateLogoutTimer();

    UFUNCTION()
    void CancelLogout();

    UFUNCTION()
    void InitiateLogout();

    UFUNCTION()
    void OnCancelLogoutButtonClicked();

    
    void ShowErrorMessage(const FString &Message);
};