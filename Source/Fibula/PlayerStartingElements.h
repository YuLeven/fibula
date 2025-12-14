#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameItem.h"
#include "CharacterEnums.h"
#include "PlayerStartingElements.generated.h"

UCLASS()
class FIBULA_API UPlayerStartingElements : public UObject
{
    GENERATED_BODY()

public:
    static TArray<FGameItem> GetStartingItems(EVocation Vocation);
    static int32 GetStartingLevel() { return 100; }
    static int32 GetStartingMagicLevel(EVocation Vocation);
    static int32 GetStartingMeleeSkill(EVocation Vocation);
    static int32 GetStartingDistanceSkill(EVocation Vocation);
    static int32 GetStartingShieldingSkill(EVocation Vocation);
    static TArray<FGameItem> GetTeamStashStartingItems();
};