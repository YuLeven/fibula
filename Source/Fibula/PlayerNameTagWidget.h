#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Curves/CurveLinearColor.h"
#include "PlayerNameTagWidget.generated.h"

UENUM()
enum class ESkullType : uint8
{
    None,
    White,
    Red,
    Black
};

UCLASS()
class FIBULA_API UPlayerNameTagWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPlayerNameTagWidget(const FObjectInitializer &ObjectInitializer);
    void SetCharacterName(const FString &Name);
    void UpdateHealth(float HealthPercentage);
    void UpdateSkullStatus(ESkullType SkullType);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock *NameText;

    UPROPERTY(meta = (BindWidget))
    UProgressBar *HealthBar;

    UPROPERTY(meta = (BindWidget))
    UImage *SkullImage;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    UCurveLinearColor *HealthColorCurve;

private:
    UPROPERTY()
    UTexture2D *WhiteSkullTexture;

    UPROPERTY()
    UTexture2D *RedSkullTexture;

    UPROPERTY()
    UTexture2D *BlackSkullTexture;
};