#include "FloatingNumberUtils.h"
#include "Kismet/GameplayStatics.h"
#include "FibulaHUD.h"
#include "FloatingNumberWidget.h"

void UFloatingNumberUtils::ShowFloatingNumber(APlayerController *LocalPC, AActor *Target, int32 Number, const FLinearColor &Color, FVector Offset)
{
    if (!LocalPC || !Target)
    {
        return;
    }

    if (AFibulaHUD *HUD = Cast<AFibulaHUD>(LocalPC->GetHUD()))
    {
        if (TSubclassOf<UFloatingNumberWidget> FloatingNumberClass = HUD->GetFloatingNumberWidgetClass())
        {
            if (UFloatingNumberWidget *NumberWidget = CreateWidget<UFloatingNumberWidget>(LocalPC, FloatingNumberClass))
            {
                NumberWidget->SetNumber(Number, Color);
                NumberWidget->AddToViewport();

                FVector2D ScreenPosition;
                if (UGameplayStatics::ProjectWorldToScreen(LocalPC, Target->GetActorLocation() + Offset, ScreenPosition))
                {
                    NumberWidget->SetPositionInViewport(ScreenPosition);
                    NumberWidget->StartFloat();
                }
            }
        }
    }
}