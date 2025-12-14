#pragma once

#include "CoreMinimal.h"
#include "FibulaGameState.h"
#include "GameModeType.h"
#include "FibulaTeamBattleGameState.generated.h"

UCLASS()
class FIBULA_API AFibulaTeamBattleGameState : public AFibulaGameState
{
    GENERATED_BODY()

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
    virtual EGameModeType GetCurrentGameMode() const override;

    float GetRemainingBattleTime() const;
    int32 GetTeamPoints(int32 TeamId) const;
    bool IsBattleActive() const;
    void SetTeamPoints(int32 TeamId, int32 Points);
    void SetBattleStartTime(double Time);
    void OnPlayerDeath(int32 TeamId);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastOnBattleEnd(int32 WinningTeamId);

private:
    UPROPERTY(Replicated)
    double BattleStartTime;

    UPROPERTY(Replicated)
    int32 TeamOnePoints;

    UPROPERTY(Replicated)
    int32 TeamTwoPoints;
};