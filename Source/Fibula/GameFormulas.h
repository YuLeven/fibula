#pragma once

#include "CoreMinimal.h"
#include "CharacterEnums.h"
#include "GameModeType.h"

class AFibulaCharacter;
struct FSpellDefinition;

namespace GameFormulas
{
    
    int32 CalculateLevel(int64 Experience);
    
    int64 CalculateExperienceForLevel(int32 TargetLevel);

    
    int32 CalculatePaladinMaxHealth(int32 Level);
    int32 CalculateKnightMaxHealth(int32 Level);
    int32 CalculateMageMaxHealth(int32 Level);

    
    int32 CalculatePaladinMaxMana(int32 Level);
    int32 CalculateKnightMaxMana(int32 Level);
    int32 CalculateMageMaxMana(int32 Level);

    
    int32 CalculatePaladinCapacity(int32 Level);
    int32 CalculateKnightCapacity(int32 Level);
    int32 CalculateMageCapacity(int32 Level);

    
    int32 CalculateSpeed(int32 Level, EVocation Vocation);

    
    int32 CalculateMaxHealth(EVocation Vocation, int32 Level);
    int32 CalculateMaxMana(EVocation Vocation, int32 Level);
    int32 CalculateMaxCapacity(EVocation Vocation, int32 Level);

    
    int32 CalculateSpellMinDamage(const AFibulaCharacter *Caster, const FSpellDefinition *Spell);
    int32 CalculateSpellMaxDamage(const AFibulaCharacter *Caster, const FSpellDefinition *Spell);
    int32 CalculateSpellEffect(AFibulaCharacter *Caster, AFibulaCharacter *Target, const FSpellDefinition *Spell);
    int32 CalculateSupportSpellEffect(AFibulaCharacter *Caster, const FSpellDefinition *Spell, EGameModeType GameMode);
    int32 GetSpellSkillLevel(const AFibulaCharacter *Caster, const FSpellDefinition *Spell);

    
    int32 CalculateAutoAttackDamage(AFibulaCharacter *Attacker, AFibulaCharacter *Target);
    int32 CalculateDamageAttenuation(AFibulaCharacter *Target);

    float GameModeHealingMultiplier(EGameModeType GameMode, EVocation Vocation);
}