#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterEnums.h"
#include "DamageTypes.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "GameModeType.h"
#include "Sound/SoundCue.h"
#include "SpellSystem.generated.h"

class AFibulaCharacter;

UENUM(BlueprintType)
enum class ESpellType : uint8
{
    InstantAreaSpell UMETA(DisplayName = "Instant Area Spell"),
    TargetedSpell UMETA(DisplayName = "Targeted Spell"),
    SupportSpell UMETA(DisplayName = "Support Spell"),
    TargetedRune UMETA(DisplayName = "Targeted Rune"),
    AreaRune UMETA(DisplayName = "Area Rune"),
    SupportRune UMETA(DisplayName = "Support Rune"),
    Potion UMETA(DisplayName = "Potion"),
    MagicWall UMETA(DisplayName = "Magic Wall"),
    TargetedHeal UMETA(DisplayName = "Targeted Heal"),
    InstantAreaHeal UMETA(DisplayName = "Instant Area Heal"),
    ProjectileSpell UMETA(DisplayName = "Projectile Spell")
};

USTRUCT(BlueprintType)
struct FSpellArea
{
    GENERATED_BODY()

    TArray<TArray<int32>> AreaGrid;
    FVector2D GridSize;
};

USTRUCT(BlueprintType)
struct FSpellEffect
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpeedMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsHealingEffect = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsMagicShield = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MeleeSkillMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageReceivedMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DistanceSkillMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MagicLevelMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsLight = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bBlocksHealing = false;
};

USTRUCT(BlueprintType)
struct FSpellFormula
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinMagicLevelRatio = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxMagicLevelRatio = 1.0f;
};

UENUM(BlueprintType)
enum class ESpellElement : uint8
{
    Physical UMETA(DisplayName = "Physical"),
    Fire UMETA(DisplayName = "Fire"),
    Ice UMETA(DisplayName = "Ice"),
    Energy UMETA(DisplayName = "Energy"),
    Holy UMETA(DisplayName = "Holy"),
    Healing UMETA(DisplayName = "Healing"),
};

USTRUCT(BlueprintType)
struct FSpellDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Words;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsSelfTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bMustBeTargeted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxTargetDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSpellArea Area;

    UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
    UParticleSystem *CascadeEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
    UNiagaraSystem *NiagaraEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
    bool bSpawnSingleEffect = false;

    UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
    UNiagaraSystem *NiagaraHomingEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Sound Effects")
    USoundCue *SpellSound;

    UPROPERTY(EditDefaultsOnly, Category = "Sound Effects")
    USoundCue *HomingSound;

    UPROPERTY(EditDefaultsOnly, Category = "Sound Effects")
    USoundCue *HitSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSpellEffect Effects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSpellFormula SpellFormula;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsSkillBasedSpell = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESpellType SpellType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESpellElement Element;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ManaCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString IconPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSet<EVocation> AllowedVocations;

    bool GetIsRune() const
    {
        return SpellType == ESpellType::TargetedRune ||
               SpellType == ESpellType::AreaRune ||
               SpellType == ESpellType::SupportRune ||
               SpellType == ESpellType::MagicWall;
    }

    bool IsOffensiveSpell() const
    {
        return SpellType == ESpellType::TargetedSpell ||
               SpellType == ESpellType::InstantAreaSpell ||
               SpellType == ESpellType::TargetedRune ||
               SpellType == ESpellType::AreaRune ||
               SpellType == ESpellType::ProjectileSpell;
    }

    EDamageDisplayType GetDamageDisplayType() const
    {
        switch (Element)
        {
        case ESpellElement::Physical:
            return EDamageDisplayType::Physical;
        case ESpellElement::Fire:
            return EDamageDisplayType::Fire;
        case ESpellElement::Ice:
            return EDamageDisplayType::Ice;
        case ESpellElement::Energy:
            return EDamageDisplayType::Energy;
        case ESpellElement::Holy:
            return EDamageDisplayType::Holy;
        case ESpellElement::Healing:
            return EDamageDisplayType::Healing;
        default:
            return EDamageDisplayType::Physical;
        }
    }

    FString GetDetailedDescription(const AFibulaCharacter *Character) const;
};

UCLASS()
class FIBULA_API ASpellSystem : public AActor
{
    GENERATED_BODY()

public:
    ASpellSystem();
    void ServerTryExecuteSpell(AFibulaCharacter *Caster, const FString &SpellName, const FVector &TargetLocation = FVector::ZeroVector);
    bool HasLineOfSightToLocation(const AFibulaCharacter *Caster, const FVector &TargetLocation) const;
    void OnProjectileSpellHit(const FString &SpellName, AFibulaCharacter *Caster, const FVector &TargetLocation);
    virtual void BeginPlay() override;
    virtual void BeginDestroy() override;

private:
    bool ExecuteSpellEffects(AFibulaCharacter *Caster, const FSpellDefinition *Spell, const FString &Message, const FVector &OverrideLocation = FVector::ZeroVector);
    void ApplySupportEffects(AFibulaCharacter *Caster, const FSpellDefinition *Spell);
    void ApplyHealingEffects(AFibulaCharacter *Caster, const FSpellDefinition *Spell);
    void ApplyAreaSpellEffects(AFibulaCharacter *Caster, const FSpellDefinition *Spell, const FVector &SpellCenter);
    void ApplyTargetedSpellEffects(AFibulaCharacter *Caster, const FSpellDefinition *Spell, const FVector &SpellCenter);
    void ApplyAreaHealingEffects(AFibulaCharacter *Caster, const FSpellDefinition *Spell, const FVector &SpellCenter);
    FVector DetermineSpellCenter(AFibulaCharacter *Caster, const FSpellDefinition *Spell, const FVector &OverrideLocation);

    void ShowFloatingMessage(AFibulaCharacter *Caster, const FString &Message);
    bool ValidateSpell(AFibulaCharacter *Caster, const FSpellDefinition *Spell, bool bCheckVocation);

    
    TMap<AFibulaCharacter *, TMap<FString, FTimerHandle>> ActiveEffectTimers;

    
    void ClearExistingEffect(AFibulaCharacter *Caster, const FString &SpellName);

    UPROPERTY()
    EGameModeType GameMode;
};