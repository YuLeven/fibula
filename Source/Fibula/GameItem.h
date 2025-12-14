#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "EquipmentTypes.h"
#include "GameItem.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    None,
    Consumable,
    Equipment,
    Tool,
    Rune
};

UENUM(BlueprintType)
enum class EItemUseType : uint8
{
    None,
    RuneSpell,
    Consumable,
    Equipment
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    Junk,
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary,
    Artifact
};

USTRUCT(BlueprintType, Blueprintable)
struct FIBULA_API FGameItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemRarity Rarity = EItemRarity::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsStackable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 StackCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemUseType UseType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString UseAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FEquipmentAttributes EquipmentAttributes;

    FGameItem()
        : Name(TEXT("")), Description(TEXT("")), Weight(0.0f), ItemType(EItemType::None), bIsStackable(false), StackCount(1), UseType(EItemUseType::None), UseAction(TEXT(""))
    {
    }

    bool NetSerialize(FArchive &Ar, class UPackageMap *Map, bool &bOutSuccess);

    FString GetDetailedDescription(int32 ItemsInStack = 1) const
    {
        FString DetailedDesc;

        
        DetailedDesc += FString::Printf(TEXT("Weight: %.1f oz\n"), Weight * ItemsInStack);

        
        if (ItemType == EItemType::Equipment)
        {
            if (EquipmentAttributes.bIsTwoHanded)
            {
                DetailedDesc += TEXT("Two-handed weapon\n");
            }

            if (EquipmentAttributes.Charges > 0)
            {
                DetailedDesc += FString::Printf(TEXT("Charges: %d\n"), EquipmentAttributes.Charges);
            }

            if (EquipmentAttributes.Attack > 0)
            {
                DetailedDesc += FString::Printf(TEXT("Attack: %d\n"), EquipmentAttributes.Attack);
            }
            if (EquipmentAttributes.Defense > 0)
            {
                DetailedDesc += FString::Printf(TEXT("Defense: %d\n"), EquipmentAttributes.Defense);
            }
            if (EquipmentAttributes.Armor > 0)
            {
                DetailedDesc += FString::Printf(TEXT("Armor: %d\n"), EquipmentAttributes.Armor);
            }

            if (EquipmentAttributes.MagicLevelBonus > 0)
            {
                DetailedDesc += FString::Printf(TEXT("Magic Level +%d (mages)"), EquipmentAttributes.MagicLevelBonus);
            }
            if (EquipmentAttributes.MeleeSkillBonus > 0)
            {
                DetailedDesc += FString::Printf(TEXT("Melee Skill +%d\n"), EquipmentAttributes.MeleeSkillBonus);
            }
            if (EquipmentAttributes.DistanceSkillBonus > 0)
            {
                DetailedDesc += FString::Printf(TEXT("Distance Skill +%d\n"), EquipmentAttributes.DistanceSkillBonus);
            }
            if (EquipmentAttributes.CapacityBonus > 0)
            {
                DetailedDesc += FString::Printf(TEXT("Capacity Bonus: %d\n"), EquipmentAttributes.CapacityBonus);
            }
        }

        
        if (!Description.IsEmpty())
        {
            DetailedDesc += TEXT("\n") + Description;
        }

        return DetailedDesc;
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUsed, const FGameItem &, Item);

template <>
struct TStructOpsTypeTraits<FGameItem> : public TStructOpsTypeTraitsBase2<FGameItem>
{
    enum
    {
        WithNetSerializer = true
    };
};