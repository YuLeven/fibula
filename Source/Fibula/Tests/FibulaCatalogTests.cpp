#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "FibulaTestWorld.h"
#include "SpellDatabase.h"
#include "PlayerStartingElements.h"
#include "RewardSystem.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaSpellCatalog, "Fibula.Content.Spells.DefinitionsAndLookup",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaSpellCatalog::RunTest(const FString&)
{
    GetDefault<AFibulaGameState>();
    const auto& Spells = USpellDatabase::GetAllSpells(EVocation::None);
    TestTrue(TEXT("Spell catalog loaded (not an empty passing scan)"), Spells.Num() >= 25);
    TSet<FString> Words;
    for (const auto& Pair : Spells)
    {
        const auto& Spell = Pair.Value;
        TestEqual(TEXT("Lookup by name"), USpellDatabase::GetSpell(Spell.Name), &Spell);
        TestEqual(TEXT("Case-insensitive name lookup"), USpellDatabase::GetSpell(Spell.Name.ToUpper()), &Spell);
        TestFalse(TEXT("Vocation eligibility is defined"), Spell.AllowedVocations.IsEmpty());
        TestTrue(TEXT("Mana cost is nonnegative"), Spell.ManaCost >= 0);
        TestTrue(TEXT("Formula bounds are ordered"), Spell.SpellFormula.MinMagicLevelRatio <= Spell.SpellFormula.MaxMagicLevelRatio);
        TestTrue(TEXT("Grid is populated"), Spell.Area.AreaGrid.Num() > 0);
        int32 Origins = 0;
        for (const auto& Row : Spell.Area.AreaGrid)
        {
            TestEqual(TEXT("Rectangular area grid"), Row.Num(), int32(Spell.Area.GridSize.Y));
            for (int32 Cell : Row) if (Cell == 3) ++Origins;
        }
        TestEqual(FString::Printf(TEXT("One area origin: %s"), *Spell.Name), Origins, 1);
        if (!Spell.Words.IsEmpty())
        {
            TestFalse(TEXT("Incantations are unique"), Words.Contains(Spell.Words));
            Words.Add(Spell.Words);
            TestEqual(TEXT("Incantation lookup"), USpellDatabase::GetSpellByWords(Spell.Words), &Spell);
        }
        if (Spell.GetIsRune() || Spell.SpellType == ESpellType::Potion)
            TestNotNull(FString::Printf(TEXT("Consumable item exists: %s"), *Spell.Name), UItemDatabase::GetItem(Spell.Name));
    }
    TestNull(TEXT("Unknown spell lookup"), USpellDatabase::GetSpell(TEXT("missing definition")));
    return true;
}

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FFibulaStartingSupplies, "Fibula.Content.Vocations.StartingSupplies",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
void FFibulaStartingSupplies::GetTests(TArray<FString>& Names, TArray<FString>& Commands) const
{
    for (int32 V = int32(EVocation::Knight); V <= int32(EVocation::Druid); ++V)
    {
        Names.Add(StaticEnum<EVocation>()->GetNameStringByValue(V));
        Commands.Add(FString::FromInt(V));
    }
}
bool FFibulaStartingSupplies::RunTest(const FString& Parameters)
{
    FibulaTests::FTestWorld F;
    const EVocation Vocation = EVocation(FCString::Atoi(*Parameters));
    auto* Character = F.Character(Vocation);
    const auto Supplies = UPlayerStartingElements::GetStartingItems(Vocation);
    TestTrue(TEXT("Vocation has combat supplies"), Supplies.Num() > 0);
    for (const auto& Item : Supplies)
    {
        TestNotNull(TEXT("Supply is in the item catalog"), UItemDatabase::GetItem(Item.Name));
        TestTrue(TEXT("Positive starting quantity"), Item.StackCount > 0);
        TestEqual(FString::Printf(TEXT("Initial quantity: %s"), *Item.Name), Character->GetItemCount(Item.Name), Item.StackCount);
        Character->RemoveItem(Item.Name, 1);
    }
    Character->InitializeCharacterStats(false);
    for (const auto& Item : Supplies)
        TestEqual(TEXT("Respawn tops up missing supplies"), Character->GetItemCount(Item.Name), Item.StackCount);
    Character->InitializeCharacterStats(false);
    for (const auto& Item : Supplies)
        TestEqual(TEXT("Repeated initialization does not duplicate supplies"), Character->GetItemCount(Item.Name), Item.StackCount);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaStashCatalog, "Fibula.Content.Items.TeamStashSupplies",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaStashCatalog::RunTest(const FString&)
{
    GetDefault<AFibulaGameState>();
    const auto Items = UPlayerStartingElements::GetTeamStashStartingItems();
    TestEqual(TEXT("Expected stash supply categories"), Items.Num(), 9);
    TSet<FString> Names;
    for (const auto& Item : Items)
    {
        TestFalse(TEXT("Unique stash entries"), Names.Contains(Item.Name));
        Names.Add(Item.Name);
        TestTrue(TEXT("Supplies are stackable"), Item.bIsStackable);
        TestTrue(TEXT("Positive stash quantities"), Item.StackCount > 0);
        TestNotNull(TEXT("Catalog entry exists"), UItemDatabase::GetItem(Item.Name));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaRewards, "Fibula.Gameplay.Rewards.ValidLootPool",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaRewards::RunTest(const FString&)
{
    GetDefault<AFibulaGameState>();
    // Property assertions hold for every draw; no flaky statistical frequency threshold.
    for (int32 Sample = 0; Sample < 512; ++Sample)
    {
        const FGameItem Item = URewardSystem::GenerateReward();
        TestFalse(TEXT("Reward is not empty"), Item.Name.IsEmpty());
        TestTrue(TEXT("Reward cannot contain another present"), Item.Name != TEXT("Reward Present"));
        TestNotNull(TEXT("Reward is a registered item"), UItemDatabase::GetItem(Item.Name));
        TestTrue(TEXT("Positive stack count"), Item.StackCount > 0);
        TestTrue(TEXT("Bounded stack count"), Item.StackCount <= 100);
        if (!Item.bIsStackable) TestEqual(TEXT("Equipment reward is singular"), Item.StackCount, 1);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaItemWire, "Fibula.Contracts.Items.ExistingWireFieldsRoundTrip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaItemWire::RunTest(const FString&)
{
    FGameItem Item;
    Item.Name = TEXT("Sudden Death Rune"); Item.Description = TEXT("Round trip test");
    Item.Weight = 1.25f; Item.bIsStackable = true; Item.StackCount = 137;
    Item.ItemType = EItemType::Rune; Item.UseType = EItemUseType::RuneSpell;
    Item.UseAction = TEXT("Sudden Death Rune");
    Item.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/Test/Icon.Icon")));
    TArray<uint8> Bytes;
    FMemoryWriter Writer(Bytes);
    bool Success = false;
    TestTrue(TEXT("Serialize succeeds"), Item.NetSerialize(Writer, nullptr, Success));
    TestTrue(TEXT("Serialize success flag"), Success);
    FGameItem Restored;
    FMemoryReader Reader(Bytes);
    Restored.NetSerialize(Reader, nullptr, Success);
    TestTrue(TEXT("Deserialize success flag"), Success);
    TestEqual(TEXT("Name"), Restored.Name, Item.Name);
    TestEqual(TEXT("Description"), Restored.Description, Item.Description);
    TestEqual(TEXT("Weight"), Restored.Weight, Item.Weight);
    TestEqual(TEXT("Stack count"), Restored.StackCount, Item.StackCount);
    TestEqual(TEXT("Stackability"), Restored.bIsStackable, Item.bIsStackable);
    TestEqual(TEXT("Item type"), Restored.ItemType, Item.ItemType);
    TestEqual(TEXT("Use type"), Restored.UseType, Item.UseType);
    TestEqual(TEXT("Use action"), Restored.UseAction, Item.UseAction);
    TestEqual(TEXT("Icon path"), Restored.Icon.ToString(), Item.Icon.ToString());
    TestTrue(TEXT("All serialized bytes consumed"), Reader.AtEnd());
    // Rarity/equipment attributes are a documented pre-existing wire-format gap.
    return true;
}
#endif
