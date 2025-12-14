#include "TooltipWidget.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UTooltipWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BackgroundBorder)
    {
        BackgroundBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.8f));
    }
}

void UTooltipWidget::SetTitle(const FString &Title)
{
    if (TitleText)
    {
        TitleText->SetText(FText::FromString(Title));
    }
}

void UTooltipWidget::SetDescription(const FString &Description)
{
    if (DescriptionText)
    {
        DescriptionText->SetText(FText::FromString(Description));
    }
}
