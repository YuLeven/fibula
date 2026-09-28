#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "FibulaTestWorld.h"
#include "FibulaFFAGameMode.h"
#include "FibulaTeamBattleGameMode.h"
#include "FibulaTeamBattleGameState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "TeamStash.h"
#include "SpellSystem.h"
#include "SpellDatabase.h"
#include "ItemDatabase.h"
#include "GameFormulas.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaCombatPolicies, "Fibula.Gameplay.Modes.DamageAndHealingPolicies",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaCombatPolicies::RunTest(const FString&)
{
    FibulaTests::FTestWorld F;
    auto* A = F.Character(EVocation::Knight, 1);
    auto* Ally = F.Character(EVocation::Druid, 1);
    auto* Enemy = F.Character(EVocation::Sorcerer, 2);
    auto* Team = F.Spawn<AFibulaTeamBattleGameMode>();
    auto* FFA = F.Spawn<AFibulaFFAGameMode>();
    TestFalse(TEXT("Team friendly fire"), Team->CanDamage(A, Ally));
    TestFalse(TEXT("Team self damage"), Team->CanDamage(A, A));
    TestTrue(TEXT("Team hostile damage"), Team->CanDamage(A, Enemy));
    TestTrue(TEXT("Team allied healing"), Team->CanHeal(A, Ally));
    TestTrue(TEXT("Team self healing"), Team->CanHeal(A, A));
    TestFalse(TEXT("Team hostile healing"), Team->CanHeal(A, Enemy));
    TestFalse(TEXT("Null damage target"), Team->CanDamage(A, nullptr));
    TestFalse(TEXT("Null healer"), Team->CanHeal(nullptr, A));
    TestTrue(TEXT("FFA ignores shared team ID"), FFA->CanDamage(A, Ally));
    TestFalse(TEXT("FFA self damage"), FFA->CanDamage(A, A));
    // Characterization: current FFA intentionally remains permissive here until a design change.
    TestTrue(TEXT("Current FFA healing policy"), FFA->CanHeal(A, Enemy));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaPvPEncounter, "Fibula.Interactions.TeamBattle.MeleeKillAndFriendlyFire",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaPvPEncounter::RunTest(const FString&)
{
    FibulaTests::FTestWorld F(AFibulaTeamBattleGameMode::StaticClass());
    auto* State = F.World->GetGameState<AFibulaTeamBattleGameState>();
    if (!TestNotNull(TEXT("Team Battle state exists"), State)) return false;

    F.Spawn<APlayerStart>(FVector(0, 0, 100));
    F.Spawn<APlayerStart>(FVector(10000, 0, 100));
    auto* Knight = F.JoinPlayer(TEXT("Arena Knight"), TEXT("Knight"));
    auto* Druid = F.JoinPlayer(TEXT("Arena Druid"), TEXT("Druid"));
    auto* Paladin = F.JoinPlayer(TEXT("Arena Paladin"), TEXT("Paladin"));
    if (!TestNotNull(TEXT("Knight joined through game mode"), Knight) ||
        !TestNotNull(TEXT("Druid joined through game mode"), Druid) ||
        !TestNotNull(TEXT("Paladin joined through game mode"), Paladin)) return false;

    TestEqual(TEXT("First player joins team one"), Knight->GetTeamId(), 1);
    TestEqual(TEXT("Second player joins team two"), Druid->GetTeamId(), 2);
    TestEqual(TEXT("Third player joins smaller team"), Paladin->GetTeamId(), 1);
    State->SetMatchState(MatchState::InProgress);
    State->SetBattleStartTime(State->GetServerWorldTimeSeconds());

    Druid->SetActorLocation(Knight->GetActorLocation() + FVector(100, 0, 0));
    Druid->SetInProtectionZone(false);
    const int32 StartingHealth = Druid->GetCurrentHealth();
    Knight->ServerSetTarget(Druid);
    TestTrue(TEXT("Targeting starts a Knight's melee attack"), Druid->GetCurrentHealth() < StartingHealth);
    TestEqual(TEXT("Target is the selected opponent"), Knight->GetCurrentTarget(), Druid);

    const int32 HealthAfterFirstHit = Druid->GetCurrentHealth();
    Knight->PerformAutoAttack();
    TestTrue(TEXT("A subsequent melee swing damages the selected opponent"), Druid->GetCurrentHealth() < HealthAfterFirstHit);

    const int32 AllyHealth = Paladin->GetCurrentHealth();
    Knight->ServerSetTarget(Paladin);
    Knight->PerformAutoAttack();
    TestEqual(TEXT("Same-team melee cannot damage an ally"), Paladin->GetCurrentHealth(), AllyHealth);
    TestEqual(TEXT("Melee encounter has no score until a death"), State->GetTeamPoints(1), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaHealth, "Fibula.Gameplay.Combat.HealthManaAndProtection",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaHealth::RunTest(const FString&)
{
    FibulaTests::FTestWorld F(AFibulaTeamBattleGameMode::StaticClass());
    auto* Target = F.Character(EVocation::Knight, 1);
    auto* Enemy = F.Character(EVocation::Sorcerer, 2);
    auto* Ally = F.Character(EVocation::Druid, 1);
    const int32 Max = Target->GetMaxHealth();
    Target->ServerModifyHealth(-100, Ally);
    TestEqual(TEXT("Friendly damage rejected at mutation boundary"), Target->GetCurrentHealth(), Max);
    Target->ServerModifyHealth(-100, Enemy);
    TestEqual(TEXT("Hostile damage applied"), Target->GetCurrentHealth(), Max - 100);
    Target->ServerModifyHealth(50, Enemy);
    TestEqual(TEXT("Enemy healing rejected"), Target->GetCurrentHealth(), Max - 100);
    Target->ServerModifyHealth(10000, Ally);
    TestEqual(TEXT("Healing clamps to maximum"), Target->GetCurrentHealth(), Max);
    Target->SetInProtectionZone(true);
    Target->ServerModifyHealth(-100, Enemy);
    TestEqual(TEXT("Protection prevents damage"), Target->GetCurrentHealth(), Max);
    Target->SetInProtectionZone(false);
    Target->EnableMagicShield(true);
    Target->ModifyMana(-Target->GetCurrentMana() + 60);
    Target->ServerModifyHealth(-100, Enemy);
    TestEqual(TEXT("Magic shield consumes mana first"), Target->GetCurrentMana(), 0);
    TestEqual(TEXT("Shield overflow reaches health"), Target->GetCurrentHealth(), Max - 40);
    Target->ModifyMana(100000);
    TestEqual(TEXT("Mana clamps at maximum"), Target->GetCurrentMana(), Target->GetMaxMana());
    Target->ModifyMana(-100000);
    TestEqual(TEXT("Mana clamps at zero"), Target->GetCurrentMana(), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaInventory, "Fibula.Gameplay.Inventory.StacksCapacityAndTransfers",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaInventory::RunTest(const FString&)
{
    FibulaTests::FTestWorld F;
    auto* Character = F.Character();
    Character->Inventory.Empty();
    FGameItem Item;
    Item.Name = TEXT("Test Supply"); Item.bIsStackable = true; Item.Weight = 1; Item.StackCount = 10;
    TestTrue(TEXT("Add stack"), Character->AddItem(Item));
    TestTrue(TEXT("Merge stack"), Character->AddItem(Item));
    TestEqual(TEXT("One merged stack"), Character->Inventory.Num(), 1);
    TestEqual(TEXT("Combined quantity"), Character->GetItemCount(Item.Name), 20);
    TestFalse(TEXT("Over-removal fails"), Character->RemoveItem(Item.Name, 21));
    TestEqual(TEXT("Failed removal preserves quantity"), Character->GetItemCount(Item.Name), 20);
    TestTrue(TEXT("Partial removal succeeds"), Character->RemoveItem(Item.Name, 5));
    TestEqual(TEXT("Remaining quantity"), Character->GetItemCount(Item.Name), 15);
    Item.StackCount = Character->GetMaxCapacity() + 1;
    TestFalse(TEXT("Overweight add fails"), Character->AddItem(Item));
    TestEqual(TEXT("Failed add preserves quantity"), Character->GetItemCount(Item.Name), 15);

    auto* Stash = F.Spawn<ATeamStash>(Character->GetActorLocation() + FVector(100, 0, 0));
    Item.StackCount = 80;
    Stash->SetInventory({Item});
    // Exercise the server-side handler directly: this isolated world has no
    // network driver to deliver Server RPCs.
    Character->ServerTransferItemFromContainer_Implementation(Stash, Item);
    TestEqual(TEXT("Transfer limited to 50"), Character->GetItemCount(Item.Name), 65);
    TestEqual(TEXT("Container conserves remainder"), Stash->GetInventory()[0].StackCount, 30);
    Stash->SetActorLocation(Character->GetActorLocation() + FVector(1000, 0, 0));
    Character->ServerTransferItemFromContainer_Implementation(Stash, Item);
    TestEqual(TEXT("Distant loot rejected"), Character->GetItemCount(Item.Name), 65);
    TestEqual(TEXT("Distant loot preserves container"), Stash->GetInventory()[0].StackCount, 30);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaEquipment, "Fibula.Gameplay.Inventory.EquipmentAndTwoHandedRestrictions",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaEquipment::RunTest(const FString&)
{
    FibulaTests::FTestWorld F;
    auto* C = F.Character();
    C->Inventory.Empty();
    FGameItem Weapon;
    Weapon.Name = TEXT("Test Two Hander"); Weapon.ItemType = EItemType::Equipment;
    Weapon.EquipmentAttributes.EquipmentType = EEquipmentType::Weapon;
    Weapon.EquipmentAttributes.bIsTwoHanded = true; Weapon.EquipmentAttributes.Attack = 45;
    FGameItem Shield;
    Shield.Name = TEXT("Test Shield"); Shield.ItemType = EItemType::Equipment;
    Shield.EquipmentAttributes.EquipmentType = EEquipmentType::Shield;
    Shield.EquipmentAttributes.Defense = 20;
    C->EquipItem(Weapon);
    TestTrue(TEXT("Cannot equip unowned item"), C->GetEquippedWeapon().Name.IsEmpty());
    C->AddItem(Weapon); C->AddItem(Shield); C->EquipItem(Weapon);
    TestEqual(TEXT("Equipped weapon affects attack"), C->GetAttack(), 45);
    TestEqual(TEXT("Equipping consumes inventory copy"), C->GetItemCount(Weapon.Name), 0);
    C->EquipItem(Shield);
    TestTrue(TEXT("Shield blocked by two handed weapon"), C->GetEquippedShield().Name.IsEmpty());
    TestEqual(TEXT("Rejected equip retains item"), C->GetItemCount(Shield.Name), 1);
    C->ServerUnequipItem_Implementation(Weapon);
    TestEqual(TEXT("Unequip returns weapon"), C->GetItemCount(Weapon.Name), 1);
    C->EquipItem(Shield); C->EquipItem(Weapon);
    TestTrue(TEXT("Two hander blocked by equipped shield"), C->GetEquippedWeapon().Name.IsEmpty());
    TestEqual(TEXT("Equipped shield affects defense"), C->GetDefense(), 20);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaSpells, "Fibula.Gameplay.Spells.ValidationCostsAndExhaustion",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaSpells::RunTest(const FString&)
{
    FibulaTests::FTestWorld F(AFibulaTeamBattleGameMode::StaticClass());
    auto* C = F.Character(EVocation::Sorcerer);
    auto* Enemy = F.Character(EVocation::Knight, 2);
    Enemy->SetActorLocation(C->GetActorLocation() + FVector(100, 0, 0));
    auto* GameState = F.World->GetGameState<AFibulaGameState>();
    if (!TestNotNull(TEXT("Game state owns the match spell system"), GameState) ||
        !TestNotNull(TEXT("Spell system initialized by the match"), GameState->GetSpellSystem())) return false;
    const int32 Mana = C->GetCurrentMana();
    C->ServerCastSpell(TEXT("Not A Spell"));
    TestEqual(TEXT("Unknown spell has no cost"), C->GetCurrentMana(), Mana);
    C->ServerCastSpell(TEXT("Berserk"));
    TestEqual(TEXT("Wrong vocation has no cost"), C->GetCurrentMana(), Mana);
    C->ServerCastSpell(TEXT("Sudden Death Rune"));
    TestEqual(TEXT("Missing target has no cost"), C->GetCurrentMana(), Mana);
    const FGameItem* RuneDefinition = UItemDatabase::GetItem(TEXT("Sudden Death Rune"));
    if (!TestNotNull(TEXT("Rune definition loaded"), RuneDefinition)) return false;
    C->Inventory.Empty();
    FGameItem Runes = *RuneDefinition;
    Runes.StackCount = 2;
    TestTrue(TEXT("Rune can be placed in inventory"), C->AddItem(Runes));
    C->ServerSetTarget(Enemy);
    const int32 EnemyHealth = Enemy->GetCurrentHealth();
    C->ServerUseItem(Runes, FVector::ZeroVector);
    TestTrue(TEXT("Targeted rune damages its target"), Enemy->GetCurrentHealth() < EnemyHealth);
    TestEqual(TEXT("Successful rune consumes one charge"), C->GetItemCount(Runes.Name), 1);
    TestEqual(TEXT("Zero-mana rune leaves mana unchanged"), C->GetCurrentMana(), Mana);
    C->SetGeneralExhaust(false);
    C->SetOffensiveExhaust(false);
    C->ServerModifyHealth(-300);
    const int32 Health = C->GetCurrentHealth();
    C->ServerCastSpell(TEXT("Ultimate Healing"));
    TestTrue(TEXT("Healing changes health"), C->GetCurrentHealth() > Health);
    TestEqual(TEXT("Successful support cast costs mana once"), C->GetCurrentMana(), Mana - 100);
    TestTrue(TEXT("Successful cast exhausts"), C->IsGenerallyExhausted());
    C->ServerCastSpell(TEXT("Ultimate Healing"));
    TestEqual(TEXT("Exhausted cast costs nothing"), C->GetCurrentMana(), Mana - 100);
    C->SetGeneralExhaust(false);
    C->ModifyMana(-C->GetCurrentMana());
    C->ServerCastSpell(TEXT("Ultimate Healing"));
    TestEqual(TEXT("Insufficient mana stays at zero"), C->GetCurrentMana(), 0);
    TestFalse(TEXT("Rejected cast does not exhaust"), C->IsGenerallyExhausted());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaTeamScore, "Fibula.Gameplay.Modes.ScoreAndClockLifecycle",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaTeamScore::RunTest(const FString&)
{
    FibulaTests::FTestWorld F;
    auto* State = F.Spawn<AFibulaTeamBattleGameState>();
    State->SetTeamPoints(1, 0); State->SetTeamPoints(2, 0);
    State->SetMatchState(MatchState::WaitingToStart);
    State->OnPlayerDeath(1);
    TestEqual(TEXT("No warmup score"), State->GetTeamPoints(2), 0);
    State->SetMatchState(MatchState::InProgress);
    State->SetBattleStartTime(State->GetServerWorldTimeSeconds() - 10.0);
    State->OnPlayerDeath(1); State->OnPlayerDeath(2); State->OnPlayerDeath(2);
    TestEqual(TEXT("Team one scores opposing deaths"), State->GetTeamPoints(1), 2);
    TestEqual(TEXT("Team two scores opposing deaths"), State->GetTeamPoints(2), 1);
    TestEqual(TEXT("Countdown uses server time"), State->GetRemainingBattleTime(), 1490.0f);
    State->SetBattleStartTime(State->GetServerWorldTimeSeconds() - 1600.0);
    TestEqual(TEXT("Countdown clamps at zero"), State->GetRemainingBattleTime(), 0.0f);
    State->SetMatchState(MatchState::WaitingPostMatch);
    State->OnPlayerDeath(2);
    TestEqual(TEXT("No post-match score"), State->GetTeamPoints(1), 2);
    TestFalse(TEXT("Ended match is inactive"), State->IsBattleActive());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaTeamSupportEncounter, "Fibula.Interactions.TeamBattle.HealFriend",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaTeamSupportEncounter::RunTest(const FString&)
{
    FibulaTests::FTestWorld F(AFibulaTeamBattleGameMode::StaticClass());
    F.Spawn<APlayerStart>(FVector(0, 0, 100));
    F.Spawn<APlayerStart>(FVector(10000, 0, 100));
    auto* Knight = F.JoinPlayer(TEXT("Enemy Knight"), TEXT("Knight"));
    auto* Druid = F.JoinPlayer(TEXT("Team Druid"), TEXT("Druid"));
    auto* Paladin = F.JoinPlayer(TEXT("Team Paladin"), TEXT("Paladin"));
    auto* Sorcerer = F.JoinPlayer(TEXT("Team Sorcerer"), TEXT("Sorcerer"));
    if (!TestNotNull(TEXT("Enemy player joined"), Knight) ||
        !TestNotNull(TEXT("Druid joined"), Druid) ||
        !TestNotNull(TEXT("Paladin joined"), Paladin) ||
        !TestNotNull(TEXT("Sorcerer joined"), Sorcerer)) return false;

    TestEqual(TEXT("Healer and recipient join same team"), Druid->GetTeamId(), Sorcerer->GetTeamId());
    TestEqual(TEXT("Enemy joins the opposite team"), Knight->GetTeamId(), Paladin->GetTeamId());
    Sorcerer->ServerModifyHealth(-(Sorcerer->GetMaxHealth() / 2), Knight);
    const int32 WoundedHealth = Sorcerer->GetCurrentHealth();
    const int32 DruidMana = Druid->GetCurrentMana();

    Druid->ServerCastSpell(TEXT("Heal Friend"));
    TestEqual(TEXT("Heal Friend needs an explicit ally target"), Druid->GetCurrentMana(), DruidMana);
    Druid->ServerSetHealingTarget(Sorcerer);
    Druid->ServerCastSpell(TEXT("Heal Friend"));
    TestTrue(TEXT("Targeted support spell heals a wounded teammate"), Sorcerer->GetCurrentHealth() > WoundedHealth);
    TestEqual(TEXT("Team healing charges mana once"), Druid->GetCurrentMana(), DruidMana - 140);
    TestEqual(TEXT("Healing leaves the opposing player unchanged"), Knight->GetCurrentHealth(), Knight->GetMaxHealth());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaDeathLifecycle, "Fibula.Gameplay.Combat.DeathScoreAndRespawn",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaDeathLifecycle::RunTest(const FString&)
{
    FibulaTests::FTestWorld F(AFibulaTeamBattleGameMode::StaticClass());
    auto* Mode = F.World->GetAuthGameMode<AFibulaTeamBattleGameMode>();
    auto* State = F.World->GetGameState<AFibulaTeamBattleGameState>();
    if (!TestNotNull(TEXT("Team Battle mode initialized"), Mode) ||
        !TestNotNull(TEXT("Team Battle state initialized"), State)) return false;

    F.Spawn<APlayerStart>(FVector(0, 0, 100));
    F.Spawn<APlayerStart>(FVector(10000, 0, 100));
    auto* Killer = F.Character(EVocation::Knight, 1);
    auto* Victim = F.Character(EVocation::Druid, 2);
    Killer->SetIsTeamBattle(true);
    Victim->SetIsTeamBattle(true);
    State->SetMatchState(MatchState::InProgress);
    State->SetBattleStartTime(State->GetServerWorldTimeSeconds());

    Victim->ServerModifyHealth(-Victim->GetMaxHealth(), Killer);
    TestEqual(TEXT("Death is recorded once"), Victim->GetDeaths(), 1);
    TestEqual(TEXT("Enemy death scores for killer's team"), State->GetTeamPoints(1), 1);
    TestEqual(TEXT("Victim respawns at full health"), Victim->GetCurrentHealth(), Victim->GetMaxHealth());
    TestTrue(TEXT("Respawn grants protection"), Victim->IsInProtectionZone());
    Victim->ServerModifyHealth(-100, Killer);
    TestEqual(TEXT("Protection prevents immediate repeat death"), Victim->GetDeaths(), 1);
    TestEqual(TEXT("Rejected respawn damage grants no score"), State->GetTeamPoints(1), 1);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibulaFortyPlayers, "Fibula.Gameplay.Modes.FortyActorAssignment",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FFibulaFortyPlayers::RunTest(const FString&)
{
    FibulaTests::FTestWorld F;
    auto* Mode = F.Spawn<AFibulaTeamBattleGameMode>();
    F.Spawn<APlayerStart>(FVector(0, 0, 100));
    F.Spawn<APlayerStart>(FVector(10000, 0, 100));
    int32 Counts[3] = {0, 0, 0};
    for (int32 Index = 0; Index < 40; ++Index)
    {
        auto* C = F.Spawn<AFibulaCharacter>();
        auto* PC = F.Spawn<APlayerController>();
        PC->Possess(C);
        Mode->InitializePlayerCharacter(PC, FString::Printf(TEXT("Test Player %d"), Index), TEXT("Knight"));
        const int32 Team = C->GetTeamId();
        if (!TestTrue(TEXT("Assigned a valid team"), Team == 1 || Team == 2)) return false;
        Counts[Team]++;
        TestTrue(TEXT("Headcounts differ by at most one after every join"), FMath::Abs(Counts[1] - Counts[2]) <= 1);
        TestEqual(TEXT("Initialized runtime level"), C->GetCharacterLevel(), 100);
    }
    TestEqual(TEXT("Twenty players on team one"), Counts[1], 20);
    TestEqual(TEXT("Twenty players on team two"), Counts[2], 20);
    // This is a rule/actor integration test, not a 40-connection capacity benchmark.
    return true;
}
#endif
