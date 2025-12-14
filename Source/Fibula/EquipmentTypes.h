#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "EquipmentTypes.generated.h"

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
    None,
    Helm,
    Armor,
    Legs,
    Boots,
    Weapon,
    Shield,
    Amulet,
    Ring,
    Ammunition,
    Bag
};

USTRUCT(BlueprintType)
struct FIBULA_API FEquipmentAttributes
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EEquipmentType EquipmentType = EEquipmentType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsTwoHanded = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanFireProjectiles = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UNiagaraSystem *ProjectileEffect = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Armor = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Attack = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Defense = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MagicLevelBonus = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 DistanceSkillBonus = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MeleeSkillBonus = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpeedBonus = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageReductionPercent = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Charges = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bPreventItemLoss = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CapacityBonus = 0;
};