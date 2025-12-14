#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "TooltipWidget.generated.h"

UCLASS()
class FIBULA_API UTooltipWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetTitle(const FString &Title);
    void SetDescription(const FString &Description);
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    class UCanvasPanel *RootCanvas;

    UPROPERTY(meta = (BindWidget))
    class UBorder *BackgroundBorder;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock *TitleText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock *DescriptionText;
};