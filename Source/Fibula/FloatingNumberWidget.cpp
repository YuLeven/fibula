#include "FloatingNumberWidget.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UFloatingNumberWidget::NativeConstruct()
{
    Super::NativeConstruct();

    
    if (!NumberText)
    {
        NumberText = Cast<UTextBlock>(GetWidgetFromName(TEXT("NumberText")));
    }
}

void UFloatingNumberWidget::SetNumber(int32 Number, const FLinearColor &Color)
{
    if (NumberText)
    {
        NumberText->SetText(FText::AsNumber(Number));
        NumberText->SetColorAndOpacity(FSlateColor(Color));
    }
}

void UFloatingNumberWidget::StartFloat()
{
    InitialPosition = GetCachedGeometry().GetAbsolutePosition();
    CurrentLifetime = 0.0f;
}

void UFloatingNumberWidget::NativeTick(const FGeometry &MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    CurrentLifetime += InDeltaTime;

    if (CurrentLifetime >= MaxLifetime)
    {
        RemoveFromParent();
        return;
    }

    
    float Alpha = CurrentLifetime / MaxLifetime;
    FVector2D NewPosition = InitialPosition + FVector2D(0, -FloatSpeed * CurrentLifetime);

    
    SetRenderTranslation(NewPosition - InitialPosition);
    SetRenderOpacity(1.0f - Alpha);
}