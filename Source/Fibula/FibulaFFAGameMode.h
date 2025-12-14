#pragma once

#include "CoreMinimal.h"
#include "FibulaGameMode.h"
#include "FibulaFFAGameMode.generated.h"

/**
 * Free For All game mode where players can attack anyone and only heal themselves
 */
UCLASS()
class FIBULA_API AFibulaFFAGameMode : public AFibulaGameMode
{
    GENERATED_BODY()

public:
    AFibulaFFAGameMode();

    
    virtual bool CanDamage(AFibulaCharacter *Attacker, AFibulaCharacter *Target) const override;
    virtual bool CanHeal(AFibulaCharacter *Healer, AFibulaCharacter *Target) const override;
    virtual AActor *FindTeamPlayerStart(int32 TeamId) override;
    virtual void AssignTeam(AFibulaCharacter *Character) override;
};