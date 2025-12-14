#pragma once

#include "CharacterEnums.generated.h"

UENUM(BlueprintType)
enum class EVocation : uint8
{
    None UMETA(DisplayName = "None"),
    Knight UMETA(DisplayName = "Knight"),
    Paladin UMETA(DisplayName = "Paladin"),
    Sorcerer UMETA(DisplayName = "Sorcerer"),
    Druid UMETA(DisplayName = "Druid")
};
