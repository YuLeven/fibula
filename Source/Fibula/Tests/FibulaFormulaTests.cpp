#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "GameFormulas.h"
#include "PlayerStartingElements.h"
#include "FibulaTestWorld.h"
#include "SpellDatabase.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FFibulaVocationBaseline,
    "Fibula.Rules.Vocations.Level100", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FFibulaVocationBaseline::GetTests(TArray<FString>& Names, TArray<FString>& Commands) const
{
    for (const TCHAR* Name : {TEXT("Knight"), TEXT("Paladin"), TEXT("Sorcerer"), TEXT("Druid")})
    {
        Names.Add(Name);
        Commands.Add(Name);
    }
}

bool FFibulaVocationBaseline::RunTest(const FString& Parameters)
{
    struct FBaseline { const TCHAR* Name; EVocation Vocation; int32 Health, Mana, Capacity, Speed, Magic, Melee, Distance, Shielding; };
    const FBaseline Baselines[] = {
        {TEXT("Knight"), EVocation::Knight, 1565, 550, 2770, 511, 8, 65, 20, 80},
        {TEXT("Paladin"), EVocation::Paladin, 1105, 1470, 2310, 409, 21, 10, 65, 40},
        {TEXT("Sorcerer"), EVocation::Sorcerer, 645, 2850, 1390, 409, 65, 10, 10, 20},
        {TEXT("Druid"), EVocation::Druid, 645, 2850, 1390, 409, 65, 10, 10, 20}
    };
    for (const auto& B : Baselines)
    {
        if (Parameters != B.Name) continue;
        TestEqual(TEXT("Starting level"), UPlayerStartingElements::GetStartingLevel(), 100);
        TestEqual(TEXT("Health"), GameFormulas::CalculateMaxHealth(B.Vocation, 100), B.Health);
        TestEqual(TEXT("Mana"), GameFormulas::CalculateMaxMana(B.Vocation, 100), B.Mana);
        TestEqual(TEXT("Capacity"), GameFormulas::CalculateMaxCapacity(B.Vocation, 100), B.Capacity);
        TestEqual(TEXT("Speed"), GameFormulas::CalculateSpeed(100, B.Vocation), B.Speed);
        TestEqual(TEXT("Magic"), UPlayerStartingElements::GetStartingMagicLevel(B.Vocation), B.Magic);
        TestEqual(TEXT("Melee"), UPlayerStartingElements::GetStartingMeleeSkill(B.Vocation), B.Melee);
        TestEqual(TEXT("Distance"), UPlayerStartingElements::GetStartingDistanceSkill(B.Vocation), B.Distance);
        TestEqual(TEXT("Shielding"), UPlayerStartingElements::GetStartingShieldingSkill(B.Vocation), B.Shielding);
        for (int32 Level = 9; Level <= 300; ++Level)
        {
            TestTrue(TEXT("Health grows with level"), GameFormulas::CalculateMaxHealth(B.Vocation, Level) > GameFormulas::CalculateMaxHealth(B.Vocation, Level - 1));
            TestTrue(TEXT("Mana grows with level"), GameFormulas::CalculateMaxMana(B.Vocation, Level) > GameFormulas::CalculateMaxMana(B.Vocation, Level - 1));
        }
        return true;
    }
    AddError(TEXT("Unknown vocation test case"));
    return false;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaExperienceCurve, "Fibula.Rules.Progression.LevelBoundaries",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaExperienceCurve::RunTest(const FString&)
{
    TestEqual(TEXT("Minimum level"), GameFormulas::CalculateLevel(0), 8);
    TestEqual(TEXT("Level 100 experience baseline"), GameFormulas::CalculateExperienceForLevel(100), int64(15694800));
    for (int32 Level = 9; Level <= 300; ++Level)
    {
        const int64 Threshold = GameFormulas::CalculateExperienceForLevel(Level);
        TestEqual(TEXT("At level boundary"), GameFormulas::CalculateLevel(Threshold), Level);
        TestEqual(TEXT("Before level boundary"), GameFormulas::CalculateLevel(Threshold - 1), Level - 1);
        TestEqual(TEXT("After level boundary"), GameFormulas::CalculateLevel(Threshold + 1), Level);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaHealingModes, "Fibula.Rules.Balance.HealingByMode",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaHealingModes::RunTest(const FString&)
{
    for (EVocation V : {EVocation::Knight, EVocation::Paladin, EVocation::Sorcerer, EVocation::Druid})
    {
        TestEqual(TEXT("Team healing is unscaled"), GameFormulas::GameModeHealingMultiplier(EGameModeType::TeamBattle, V), 1.0f);
        TestEqual(TEXT("FFA healing baseline"), GameFormulas::GameModeHealingMultiplier(EGameModeType::FreeForAll, V),
            V == EVocation::Knight || V == EVocation::Paladin ? 0.5f : 0.3f);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaSpellBounds, "Fibula.Rules.Balance.SpellAndAttackBounds",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaSpellBounds::RunTest(const FString&)
{
    FibulaTests::FTestWorld Fixture;
    auto* Caster = Fixture.Character(EVocation::Sorcerer);
    auto* Target = Fixture.Character(EVocation::Knight, 2);
    const auto* Spell = USpellDatabase::GetSpell(TEXT("Ultimate Healing"));
    if (!TestNotNull(TEXT("Healing definition loaded"), Spell)) return false;
    TestEqual(TEXT("Minimum healing at baseline"), GameFormulas::CalculateSpellMinDamage(Caster, Spell), 652);
    TestEqual(TEXT("Maximum healing at baseline"), GameFormulas::CalculateSpellMaxDamage(Caster, Spell), 1047);
    for (int32 Sample = 0; Sample < 256; ++Sample)
    {
        const int32 Effect = GameFormulas::CalculateSupportSpellEffect(Caster, Spell, EGameModeType::TeamBattle);
        TestTrue(TEXT("Healing remains inside advertised bounds"), Effect >= 652 && Effect <= 1047);
    }
    TestEqual(TEXT("Missing caster cannot cause damage"), GameFormulas::CalculateAutoAttackDamage(nullptr, Target), 0);
    TestEqual(TEXT("Missing spell has no effect"), GameFormulas::CalculateSpellEffect(Caster, Target, nullptr), 0);
    return true;
}
#endif
