#include "GameFormulas.h"
#include "Math/UnrealMathUtility.h"
#include "FibulaCharacter.h"
#include "SpellSystem.h"

namespace GameFormulas
{
    int32 CalculateLevel(int64 Experience)
    {
        int32 Level = 1;
        int64 TotalExp = 0;

        while (true)
        {
            int64 ExpForNextLevel = static_cast<int64>(50.0 * FMath::Pow(static_cast<double>(Level), 2.0) - 150.0 * Level + 200.0);
            if (TotalExp + ExpForNextLevel > Experience)
                break;

            TotalExp += ExpForNextLevel;
            Level++;
        }

        return FMath::Max(8, Level); 
    }

    int64 CalculateExperienceForLevel(int32 TargetLevel)
    {
        int64 TotalExp = 0;
        for (int32 i = 1; i < TargetLevel; ++i)
        {
            TotalExp += static_cast<int64>(50.0 * FMath::Pow(static_cast<double>(i), 2.0) - 150.0 * i + 200.0);
        }
        return TotalExp;
    }

    int32 CalculatePaladinMaxHealth(int32 Level)
    {
        return 5 * (2 * Level + 21);
    }

    int32 CalculateKnightMaxHealth(int32 Level)
    {
        return 5 * (3 * Level + 13);
    }

    int32 CalculateMageMaxHealth(int32 Level)
    {
        return 5 * (Level + 29);
    }

    int32 CalculatePaladinMaxMana(int32 Level)
    {
        return 5 * (3 * Level - 6);
    }

    int32 CalculateKnightMaxMana(int32 Level)
    {
        return 5 * (Level + 10);
    }

    int32 CalculateMageMaxMana(int32 Level)
    {
        return 5 * (6 * Level - 30);
    }

    int32 CalculatePaladinCapacity(int32 Level)
    {
        return 10 * (2 * Level + 31);
    }

    int32 CalculateKnightCapacity(int32 Level)
    {
        return 5 * (5 * Level + 54);
    }

    int32 CalculateMageCapacity(int32 Level)
    {
        return 10 * (Level + 39);
    }

    int32 CalculateSpeed(int32 Level, EVocation Vocation)
    {
        int32 BaseSpeed = 209 + (2 * Level);
        if (Vocation == EVocation::Knight)
        {
            return static_cast<int32>(BaseSpeed * 1.25f);
        }
        return BaseSpeed;
    }

    int32 CalculateMaxHealth(EVocation Vocation, int32 Level)
    {
        switch (Vocation)
        {
        case EVocation::Paladin:
            return CalculatePaladinMaxHealth(Level);
        case EVocation::Knight:
            return CalculateKnightMaxHealth(Level);
        case EVocation::Sorcerer:
        case EVocation::Druid:
            return CalculateMageMaxHealth(Level);
        default:
            return 0;
        }
    }

    int32 CalculateMaxMana(EVocation Vocation, int32 Level)
    {
        switch (Vocation)
        {
        case EVocation::Paladin:
            return CalculatePaladinMaxMana(Level);
        case EVocation::Knight:
            return CalculateKnightMaxMana(Level);
        case EVocation::Sorcerer:
        case EVocation::Druid:
            return CalculateMageMaxMana(Level);
        default:
            return 0;
        }
    }

    int32 CalculateMaxCapacity(EVocation Vocation, int32 Level)
    {
        switch (Vocation)
        {
        case EVocation::Paladin:
            return CalculatePaladinCapacity(Level);
        case EVocation::Knight:
            return CalculateKnightCapacity(Level);
        case EVocation::Sorcerer:
        case EVocation::Druid:
            return CalculateMageCapacity(Level);
        default:
            return 0;
        }
    }

    int32 CalculateSpellMinDamage(const AFibulaCharacter *Caster, const FSpellDefinition *Spell)
    {
        if (!Caster || !Spell)
            return 0;

        const int32 CasterLevel = Caster->GetCharacterLevel();
        int32 SkillLevel = GetSpellSkillLevel(Caster, Spell);
        const float BaseFormula = ((CasterLevel * 2) + (SkillLevel * 3)) * 0.5f;
        const float MinDamage = BaseFormula * Spell->SpellFormula.MinMagicLevelRatio;

        return FMath::RoundToInt(MinDamage);
    }

    int32 CalculateSpellMaxDamage(const AFibulaCharacter *Caster, const FSpellDefinition *Spell)
    {
        if (!Caster || !Spell)
            return 0;

        const int32 CasterLevel = Caster->GetCharacterLevel();
        const int32 SkillLevel = GetSpellSkillLevel(Caster, Spell);
        const float BaseFormula = ((CasterLevel * 2) + (SkillLevel * 3)) * 0.5f;
        const float MaxDamage = BaseFormula * Spell->SpellFormula.MaxMagicLevelRatio;

        return FMath::RoundToInt(MaxDamage);
    }

    int32 CalculateSpellEffect(AFibulaCharacter *Caster, AFibulaCharacter *Target, const FSpellDefinition *Spell)
    {
        if (!Caster || !Target || !Spell)
            return 0;

        const int32 SkillLevel = GetSpellSkillLevel(Caster, Spell);
        const int32 Attenuation = Spell->Element == ESpellElement::Physical ? CalculateDamageAttenuation(Target) : 0;

        UE_LOG(LogTemp, Log, TEXT("Damage Attenuation: %d"), Attenuation);

        const int32 CasterLevel = Caster->GetCharacterLevel();
        const float MinDamage = CalculateSpellMinDamage(Caster, Spell);
        const float MaxDamage = CalculateSpellMaxDamage(Caster, Spell);

        return FMath::RandRange(FMath::RoundToInt(MinDamage), FMath::RoundToInt(MaxDamage)) - Attenuation;
    }

    int32 CalculateSupportSpellEffect(AFibulaCharacter *Caster, const FSpellDefinition *Spell, EGameModeType GameMode)
    {
        if (!Caster || !Spell)
            return 0;

        const int32 SkillLevel = GetSpellSkillLevel(Caster, Spell);
        const int32 CasterLevel = Caster->GetCharacterLevel();
        const float HealingMultiplier = GameModeHealingMultiplier(GameMode, Caster->GetVocation());
        const float MinDamage = CalculateSpellMinDamage(Caster, Spell) * HealingMultiplier;
        const float MaxDamage = CalculateSpellMaxDamage(Caster, Spell) * HealingMultiplier;

        return FMath::RandRange(FMath::RoundToInt(MinDamage), FMath::RoundToInt(MaxDamage));
    }

    int32 CalculateAutoAttackDamage(AFibulaCharacter *Attacker, AFibulaCharacter *Target)
    {
        if (!Attacker || !Target)
            return 0;

        const int32 Level = Attacker->GetCharacterLevel();
        const int32 Skill = (Attacker->GetVocation() == EVocation::Knight) ? Attacker->GetMeleeSkill() : Attacker->GetDistanceSkill();
        const int32 Attack = FMath::Max(Attacker->GetAttack(), 5);

        const int32 MinDamage = (Level + Attack) / 15;
        const int32 MaxDamage = FMath::RoundToInt(0.09f * Skill * (Attack / 2)) + MinDamage;
        const int32 Attenuation = CalculateDamageAttenuation(Target);

        return FMath::RandRange(MinDamage, MaxDamage) - Attenuation;
    }

    int32 CalculateDamageAttenuation(AFibulaCharacter *Target)
    {
        if (!Target)
            return 0;

        const int32 TargetDefense = Target->GetDefense();
        const int32 TargetArmor = Target->GetArmor();
        const int32 TargetShieldingSkill = Target->GetShieldingSkill();
        const int32 TotalDefense = FMath::RoundToInt((TargetDefense + TargetArmor) * (TargetShieldingSkill / 100.0f));

        return FMath::RandRange(TargetArmor, TotalDefense);
    }

    int32 GetSpellSkillLevel(const AFibulaCharacter *Caster, const FSpellDefinition *Spell)
    {
        if (!Caster || !Spell)
            return 0;

        if (!Spell->bIsSkillBasedSpell)
            return Caster->GetMagicLevel();

        int32 WeaponAttack = Caster->GetAttack();
        switch (Caster->GetVocation())
        {
        case EVocation::Paladin:
            return (FMath::Pow(WeaponAttack, 1.4) + FMath::Pow(Caster->GetDistanceSkill(), 1.4)) / 4;
        case EVocation::Knight:
            return (FMath::Pow(WeaponAttack, 1.4) + FMath::Pow(Caster->GetMeleeSkill(), 1.4)) / 4;
        default:
            return Caster->GetMagicLevel();
        }
    }

    float GameModeHealingMultiplier(EGameModeType GameMode, EVocation Vocation)
    {
        if (Vocation == EVocation::Paladin || Vocation == EVocation::Knight)
            return GameMode == EGameModeType::FreeForAll ? 0.5 : 1.0;

        return GameMode == EGameModeType::FreeForAll ? 0.3 : 1.0;
    }
}