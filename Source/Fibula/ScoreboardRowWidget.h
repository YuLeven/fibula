#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "FibulaCharacter.h"
#include "ScoreboardRowWidget.generated.h"

UCLASS()
class FIBULA_API UScoreboardRowWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetupRow(AFibulaCharacter *Player);
    FLinearColor GetTeamColor(int32 TeamId) const;

protected:
    UPROPERTY(meta = (BindWidget))
    UHorizontalBox *ContentBox;

    UPROPERTY(meta = (BindWidget))
    UBorder *Background;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *NameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *LevelText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *KillsText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *DeathsText;

    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    FLinearColor TeamOneColor = FLinearColor(0.7f, 0.2f, 0.2f, 0.3f);

    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    FLinearColor TeamTwoColor = FLinearColor(0.2f, 0.2f, 0.7f, 0.3f);
};