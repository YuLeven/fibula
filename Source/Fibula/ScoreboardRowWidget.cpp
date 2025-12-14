#include "ScoreboardRowWidget.h"
#include "FibulaPlayerState.h"

void UScoreboardRowWidget::SetupRow(AFibulaCharacter *Player)
{
    if (!Player)
        return;

    AFibulaPlayerState *PlayerState = Cast<AFibulaPlayerState>(Player->GetPlayerState());
    if (!PlayerState)
        return;

    if (Background)
    {
        Background->SetBrushColor(GetTeamColor(Player->GetTeamId()));
    }

    if (NameText)
    {
        NameText->SetText(FText::FromString(Player->GetCharacterName()));
    }

    if (LevelText)
    {
        LevelText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Player->GetCharacterLevel())));
    }

    if (KillsText)
    {
        KillsText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Player->GetKills())));
    }

    if (DeathsText)
    {
        DeathsText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Player->GetDeaths())));
    }
}

FLinearColor UScoreboardRowWidget::GetTeamColor(int32 TeamId) const
{
    return TeamId == 1 ? TeamOneColor : TeamTwoColor;
}