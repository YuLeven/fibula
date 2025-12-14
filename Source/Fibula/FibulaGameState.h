#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/GameState.h"
#include "SpellSystem.h"
#include "ChatSystem.h"
#include "GameModeType.h"
#include "BackgroundMusicManager.h"
#include "FibulaGameState.generated.h"

USTRUCT()
struct FCharacterStats
{
    GENERATED_BODY()

    int32 AccumulatedKills;
    int32 AccumulatedDeaths;
    int32 MatchKills;
    int32 MatchDeaths;
    int32 Level;

    FCharacterStats()
        : AccumulatedKills(0), AccumulatedDeaths(0), MatchKills(0), MatchDeaths(0), Level(0)
    {
    }
};

UCLASS()
class FIBULA_API AFibulaGameState : public AGameState
{
    GENERATED_BODY()

public:
    AFibulaGameState();

    ASpellSystem *GetSpellSystem() const { return SpellSystem; }
    AChatSystem *GetChatSystem() const { return ChatSystem; }
    ABackgroundMusicManager *GetBackgroundMusicManager() const { return BackgroundMusicManager; }
    virtual EGameModeType GetCurrentGameMode() const;

    
    static AFibulaGameState *FindProxyGameState(UWorld *World);

    void AddCharacterKill(AFibulaCharacter *Character);
    void AddCharacterDeath(AFibulaCharacter *Character);
    TMap<FString, FCharacterStats> GetCharactersStats();
    void ResetCharactersStats();

protected:
    virtual void BeginPlay() override;

private:
    ASpellSystem *SpellSystem;
    AChatSystem *ChatSystem;
    ABackgroundMusicManager *BackgroundMusicManager;

    TMap<FString, FCharacterStats> CharacterStats;
};