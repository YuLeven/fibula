#include "FibulaFFAGameMode.h"
#include "FibulaCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameModeType.h"

AFibulaFFAGameMode::AFibulaFFAGameMode()
{
    CurrentGameMode = EGameModeType::FreeForAll;
}

bool AFibulaFFAGameMode::CanDamage(AFibulaCharacter *Attacker, AFibulaCharacter *Target) const
{
    if (!Attacker || !Target)
        return false;

    
    return Attacker != Target;
}

bool AFibulaFFAGameMode::CanHeal(AFibulaCharacter *Healer, AFibulaCharacter *Target) const
{
    return true;
}

AActor *AFibulaFFAGameMode::FindTeamPlayerStart(int32 TeamId)
{
    TArray<AActor *> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

    // If we don't have any spawn points, log an error and return nullptr
    if (PlayerStarts.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No player starts found!"));
        return nullptr;
    }

    // In FFA, we randomly select any available spawn point
    int32 RandomIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
    return PlayerStarts[RandomIndex];
}

void AFibulaFFAGameMode::AssignTeam(AFibulaCharacter *Character)
{
    if (!Character)
        return;

    // In FFA, everyone is on their own team (team 0)
    Character->SetTeamId(0);
}