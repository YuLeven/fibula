#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "ScoreboardRowWidget.h"
#include "ScoreboardWidget.generated.h"

class AFibulaCharacter;

UCLASS()
class FIBULA_API UScoreboardWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UScoreboardWidget(const FObjectInitializer &ObjectInitializer);

    void ShowVictory();
    void ShowDefeat();
    void ShowDraw();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox *PlayerList;

    UPROPERTY(meta = (BindWidget))
    UTextBlock *TitleText;

private:
    FTimerHandle UpdateTimerHandle;
    static constexpr float UPDATE_INTERVAL = 5.0f;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UScoreboardRowWidget> ScoreboardRowClass;

    void UpdateScoreboard();
    void SortPlayersByKills(TArray<AFibulaCharacter *> &Players);
};