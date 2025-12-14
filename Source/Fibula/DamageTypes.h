#pragma once

#include "CoreMinimal.h"
#include "DamageTypes.generated.h"

UENUM(BlueprintType)
enum class EDamageDisplayType : uint8
{
    Physical UMETA(DisplayName = "Physical"),
    Fire UMETA(DisplayName = "Fire"),
    Energy UMETA(DisplayName = "Energy"),
    Ice UMETA(DisplayName = "Ice"),
    Holy UMETA(DisplayName = "Holy"),
    MagicShield UMETA(DisplayName = "MagicShield"),
    Healing UMETA(DisplayName = "Healing"),
    Experience UMETA(DisplayName = "Experience")
};