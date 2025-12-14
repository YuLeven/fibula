#include "ScoreboardWidget.h"
#include "FibulaGameMode.h"
#include "FibulaPlayerState.h"
#include "FibulaGameState.h"
#include "FibulaCharacter.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "ScoreboardRowWidget.h"
#include "EngineUtils.h"

UScoreboardWidget::UScoreboardWidget(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<UScoreboardRowWidget> ScoreboardRowBPClass(TEXT("/Game/HUDs/Widgets/WBP_ScoreboardRow"));
    if (ScoreboardRowBPClass.Class != nullptr)
    {
        ScoreboardRowClass = ScoreboardRowBPClass.Class;
    }
}

void UScoreboardWidget::ShowVictory()
{
    if (TitleText)
    {
        TitleText->SetText(FText::FromString(TEXT("VICTORY!")));
        TitleText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.84f, 0.0f, 1.0f))); // Golden color
    }
}

void UScoreboardWidget::ShowDefeat()
{
    if (TitleText)
    {
        TitleText->SetText(FText::FromString(TEXT("DEFEAT!")));
        TitleText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f))); // Red color
    }
}

void UScoreboardWidget::ShowDraw()
{
    if (TitleText)
    {
        TitleText->SetText(FText::FromString(TEXT("DRAW!")));
        TitleText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 0.0f, 1.0f))); // Yellow color
    }
}

void UScoreboardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Initial update
    UpdateScoreboard();

    // Start periodic updates
    if (UWorld *World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            UpdateTimerHandle,
            this,
            &UScoreboardWidget::UpdateScoreboard,
            UPDATE_INTERVAL,
            true); // true for looping
    }
}

void UScoreboardWidget::NativeDestruct()
{
    Super::NativeDestruct();

    
    if (UWorld *World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(UpdateTimerHandle);
    }
}

void UScoreboardWidget::SortPlayersByKills(TArray<AFibulaCharacter *> &Players)
{
    Players.Sort([](const AFibulaCharacter &A, const AFibulaCharacter &B)
                 { return A.GetKills() > B.GetKills(); });
}

void UScoreboardWidget::UpdateScoreboard()
{
    if (!PlayerList || !ScoreboardRowClass)
        return;

    PlayerList->ClearChildren();

    TArray<AFibulaCharacter *> Players;

    
    for (TActorIterator<AFibulaCharacter> It(GetWorld()); It; ++It)
    {
        AFibulaCharacter *Character = *It;
        if (!Character || !Character->GetPlayerState())
            continue;

        Players.Add(Character);
    }

    SortPlayersByKills(Players);

    for (AFibulaCharacter *Player : Players)
    {
        UScoreboardRowWidget *RowWidget = CreateWidget<UScoreboardRowWidget>(this, ScoreboardRowClass);
        if (RowWidget)
        {
            RowWidget->SetupRow(Player);
            PlayerList->AddChild(RowWidget);
        }
    }
}