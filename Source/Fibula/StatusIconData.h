#pragma once

#include "CoreMinimal.h"
#include "StatusIconData.generated.h"

UENUM(BlueprintType)
enum class EStatusIconType : uint8
{
    None,
    InBattle,
    MagicShield,
    Haste,
    Paralysed,
    WhiteSkull,
    RedSkull,
    BlackSkull,
    InProtectionZone,
    Strengthened
};

USTRUCT(BlueprintType)
struct FStatusIconData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EStatusIconType StatusType = EStatusIconType::None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UTexture2D *IconTexture = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText TooltipText;

    bool operator==(const FStatusIconData &Other) const
    {
        return StatusType == Other.StatusType;
    }
};