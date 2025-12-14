#include "PlayerNameTagWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

UPlayerNameTagWidget::UPlayerNameTagWidget(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UCurveLinearColor> CurveFinder(TEXT("/Game/HUDs/Widgets/HealthBarCurveLinearColour"));
    if (CurveFinder.Succeeded())
    {
        HealthColorCurve = CurveFinder.Object;
    }

    
    static ConstructorHelpers::FObjectFinder<UTexture2D> WhiteSkullFinder(TEXT("/Game/HUDs/Images/StatusIcons/WhiteSkull"));
    static ConstructorHelpers::FObjectFinder<UTexture2D> RedSkullFinder(TEXT("/Game/HUDs/Images/StatusIcons/RedSkull"));
    static ConstructorHelpers::FObjectFinder<UTexture2D> BlackSkullFinder(TEXT("/Game/HUDs/Images/StatusIcons/BlackSkull"));

    if (WhiteSkullFinder.Succeeded())
        WhiteSkullTexture = WhiteSkullFinder.Object;
    if (RedSkullFinder.Succeeded())
        RedSkullTexture = RedSkullFinder.Object;
    if (BlackSkullFinder.Succeeded())
        BlackSkullTexture = BlackSkullFinder.Object;
}

void UPlayerNameTagWidget::SetCharacterName(const FString &Name)
{
    if (NameText)
    {
        NameText->SetText(FText::FromString(Name));
    }
}

void UPlayerNameTagWidget::UpdateHealth(float HealthPercentage)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(HealthPercentage);

        
        if (HealthColorCurve)
        {
            FLinearColor NewColor = HealthColorCurve->GetLinearColorValue(HealthPercentage);
            HealthBar->SetFillColorAndOpacity(NewColor);

            
            if (NameText)
            {
                NameText->SetColorAndOpacity(NewColor);
            }
        }
    }
}

void UPlayerNameTagWidget::UpdateSkullStatus(ESkullType SkullType)
{
    if (!SkullImage)
        return;

    UTexture2D *SkullTexture = nullptr;

    switch (SkullType)
    {
    case ESkullType::White:
        SkullTexture = WhiteSkullTexture;
        break;
    case ESkullType::Red:
        SkullTexture = RedSkullTexture;
        break;
    case ESkullType::Black:
        SkullTexture = BlackSkullTexture;
        break;
    }

    if (SkullType == ESkullType::None)
    {
        SkullImage->SetVisibility(ESlateVisibility::Hidden);
    }
    else if (SkullTexture)
    {
        SkullImage->SetBrushFromTexture(SkullTexture);
        SkullImage->SetVisibility(ESlateVisibility::Visible);
    }
}
