#include "FibulaTeamBattleGameState.h"
#include "Net/UnrealNetwork.h"
#include "SpellDatabase.h"
#include "ItemDatabase.h"
#include "FibulaCharacter.h"
#include "FibulaHUD.h"
#include "FibulaPlayerState.h"
#include "FibulaTeamBattleGameMode.h"
#include "GameFramework/GameStateBase.h"

void AFibulaTeamBattleGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFibulaTeamBattleGameState, TeamOnePoints);
    DOREPLIFETIME(AFibulaTeamBattleGameState, TeamTwoPoints);
    DOREPLIFETIME(AFibulaTeamBattleGameState, BattleStartTime);
}

EGameModeType AFibulaTeamBattleGameState::GetCurrentGameMode() const
{
    return EGameModeType::TeamBattle;
}

float AFibulaTeamBattleGameState::GetRemainingBattleTime() const
{
    if (!IsBattleActive())
        return 0.0f;

    const double CurrentTime = GetServerWorldTimeSeconds();
    const double BattleElapsedTime = CurrentTime - BattleStartTime;
    return FMath::Max(0.0f, AFibulaTeamBattleGameMode::BATTLE_DURATION - BattleElapsedTime);
}

int32 AFibulaTeamBattleGameState::GetTeamPoints(int32 TeamId) const
{
    return TeamId == 1 ? TeamOnePoints : TeamTwoPoints;
}

bool AFibulaTeamBattleGameState::IsBattleActive() const
{
    return HasMatchStarted() && !HasMatchEnded();
}

void AFibulaTeamBattleGameState::SetTeamPoints(int32 TeamId, int32 Points)
{
    if (TeamId == 1)
        TeamOnePoints = Points;
    else if (TeamId == 2)
        TeamTwoPoints = Points;
}

void AFibulaTeamBattleGameState::OnPlayerDeath(int32 TeamId)
{
    if (!HasAuthority() || !IsBattleActive())
        return;

    
    int32 ScoringTeamId = (TeamId == 1) ? 2 : 1;

    
    int32 TeamPoints = GetTeamPoints(ScoringTeamId);
    SetTeamPoints(ScoringTeamId, TeamPoints + 1);
}

void AFibulaTeamBattleGameState::SetBattleStartTime(double Time)
{
    BattleStartTime = Time;
}

void AFibulaTeamBattleGameState::MulticastOnBattleEnd_Implementation(int32 WinningTeamId)
{
    if (!HasAuthority())
    {
        APlayerController *PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            if (AFibulaHUD *HUD = Cast<AFibulaHUD>(PC->GetHUD()))
            {
                if (UStatusWidget *StatusWidget = HUD->GetStatusWidget())
                {
                    if (WinningTeamId == 0)
                    {
                        StatusWidget->ShowDrawScoreboard();
                    }
                    else
                    {
                        AFibulaCharacter *Character = Cast<AFibulaCharacter>(PC->GetPawn());
                        if (Character)
                        {
                            if (Character->GetTeamId() == WinningTeamId)
                                StatusWidget->ShowVictoryScoreboard();
                            else
                                StatusWidget->ShowDefeatScoreboard();
                        }
                    }
                }
            }
        }
    }
}
