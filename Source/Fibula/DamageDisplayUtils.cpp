#include "DamageDisplayUtils.h"
#include "Kismet/GameplayStatics.h"
#include "FibulaHUD.h"
#include "FloatingNumberUtils.h"

void UDamageDisplayUtils::ShowFloatingNumber(APlayerController *LocalPC, AActor *Target, int32 Value, EDamageDisplayType DisplayType, FVector Offset)
{
    if (!LocalPC || !Target)
    {
        return;
    }

    FLinearColor Color;
    switch (DisplayType)
    {
    case EDamageDisplayType::Physical:
        Color = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // Red
        break;
    case EDamageDisplayType::Fire:
        Color = FLinearColor(0.9f, 0.4f, 0.0f, 1.0f); // Dark Orange
        break;
    case EDamageDisplayType::Energy:
        Color = FLinearColor(0.2f, 0.6f, 1.0f, 1.0f); // Bright blue
        break;
    case EDamageDisplayType::Ice:
        Color = FLinearColor(0.4f, 0.8f, 1.0f, 1.0f); // Light blue
        break;
    case EDamageDisplayType::Holy:
        Color = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
        break;
    case EDamageDisplayType::MagicShield:
        Color = FLinearColor(0.0f, 0.0f, 0.7f, 1.0f); // Dark Blue
        break;
    case EDamageDisplayType::Healing:
        Color = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f); // Green
        break;
    case EDamageDisplayType::Experience:
        Color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); // White
        break;
    default:
        Color = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // Default to red
    }

    UFloatingNumberUtils::ShowFloatingNumber(LocalPC, Target, Value, Color, Offset);
}

void UDamageDisplayUtils::ShowFloatingNumberWithColor(APlayerController *LocalPC, AActor *Target, int32 Value, const FLinearColor &Color, FVector Offset)
{
    if (!LocalPC || !Target)
    {
        return;
    }

    UFloatingNumberUtils::ShowFloatingNumber(LocalPC, Target, Value, Color, Offset);
}