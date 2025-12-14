#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameUserSettings.h"
#include "GraphicsSettingsWidget.generated.h"

UCLASS()
class FIBULA_API UGraphicsSettingsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    class UComboBoxString *ResolutionComboBox;

    UPROPERTY(meta = (BindWidget))
    class UComboBoxString *QualityComboBox;

    UPROPERTY(meta = (BindWidget))
    class UButton *ApplyButton;

    UPROPERTY(meta = (BindWidget))
    class UButton *CloseButton;

    UPROPERTY(meta = (BindWidget))
    class UCheckBox *VSyncCheckBox;

private:
    UFUNCTION()
    void OnApplyButtonClicked();

    UFUNCTION()
    void OnCloseButtonClicked();

    void PopulateResolutionOptions();
    void PopulateQualityOptions();
    void InitializeVSyncOption();
    void ApplySettings();
};