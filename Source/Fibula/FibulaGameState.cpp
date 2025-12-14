#include "FibulaGameState.h"
#include "Net/UnrealNetwork.h"
#include "SpellDatabase.h"
#include "ItemDatabase.h"
#include "FibulaCharacter.h"
#include "FibulaHUD.h"
#include "FibulaPlayerState.h"
#include "FibulaGameMode.h"
#include "Kismet/GameplayStatics.h"

AFibulaGameState::AFibulaGameState()
{
    USpellDatabase::Initialize();
    UItemDatabase::Initialize();
}

void AFibulaGameState::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        ChatSystem = GetWorld()->SpawnActor<AChatSystem>(AChatSystem::StaticClass(), FTransform::Identity);
        SpellSystem = GetWorld()->SpawnActor<ASpellSystem>(ASpellSystem::StaticClass());
    }

    if (!HasAuthority())
    {
        if (!BackgroundMusicManager)
            BackgroundMusicManager = GetWorld()->SpawnActor<ABackgroundMusicManager>(ABackgroundMusicManager::StaticClass());

        BackgroundMusicManager->StartBattleMusic();
    }
}

EGameModeType AFibulaGameState::GetCurrentGameMode() const
{
    return EGameModeType::FreeForAll;
}

AFibulaGameState *AFibulaGameState::FindProxyGameState(UWorld *World)
{
    if (!World)
        return nullptr;

    TArray<AActor *> AllGameStates;
    UGameplayStatics::GetAllActorsOfClass(World, AFibulaGameState::StaticClass(), AllGameStates);

    for (AActor *Actor : AllGameStates)
    {
        if (AFibulaGameState *GS = Cast<AFibulaGameState>(Actor))
        {
            if (GS->IsNetMode(NM_Client))
            {
                return GS;
            }
        }
    }

    return nullptr;
}

void AFibulaGameState::AddCharacterKill(AFibulaCharacter *Character)
{
    if (!HasAuthority() || !Character)
        return;

    FCharacterStats &Stats = CharacterStats.FindOrAdd(Character->GetCharacterName());
    Stats.AccumulatedKills++;
    Stats.Level = Character->GetCharacterLevel();
    Stats.MatchKills = Character->GetKills();
}

void AFibulaGameState::AddCharacterDeath(AFibulaCharacter *Character)
{
    if (!HasAuthority() || !Character)
        return;

    FCharacterStats &Stats = CharacterStats.FindOrAdd(Character->GetCharacterName());
    Stats.Level = Character->GetCharacterLevel();
    Stats.AccumulatedDeaths++;
    Stats.MatchDeaths = Character->GetDeaths();
}

TMap<FString, FCharacterStats> AFibulaGameState::GetCharactersStats()
{
    return CharacterStats;
}

void AFibulaGameState::ResetCharactersStats()
{
    CharacterStats.Empty();
}
