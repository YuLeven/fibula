#include "PlayerStartingElements.h"
#include "ItemDatabase.h"

TArray<FGameItem> UPlayerStartingElements::GetStartingItems(EVocation Vocation)
{
    TArray<FGameItem> StartingItems;

    switch (Vocation)
    {
    case EVocation::Sorcerer:
        if (const FGameItem *ManaFluid = UItemDatabase::GetItem(TEXT("Mana Fluid")))
        {
            FGameItem NewItem = *ManaFluid;
            NewItem.StackCount = 350;
            StartingItems.Add(NewItem);
        }

        if (const FGameItem *UltimateHealingRune = UItemDatabase::GetItem(TEXT("Ultimate Healing Rune")))
        {
            FGameItem NewItem = *UltimateHealingRune;
            NewItem.StackCount = 50;
            StartingItems.Add(NewItem);
        }

        
        if (const FGameItem *SuddenDeathRune = UItemDatabase::GetItem(TEXT("Sudden Death Rune")))
        {
            FGameItem NewItem = *SuddenDeathRune;
            NewItem.StackCount = 200;
            StartingItems.Add(NewItem);
        }

        if (const FGameItem *GreatFireballRune = UItemDatabase::GetItem(TEXT("Great Fireball Rune")))
        {
            FGameItem NewItem = *GreatFireballRune;
            NewItem.StackCount = 40;
            StartingItems.Add(NewItem);
        }
        if (const FGameItem *MagicWallRune = UItemDatabase::GetItem(TEXT("Magic Wall Rune")))
        {
            FGameItem NewItem = *MagicWallRune;
            NewItem.StackCount = 40;
            StartingItems.Add(NewItem);
        }
        break;
    case EVocation::Druid:
    {
        if (const FGameItem *ManaFluid = UItemDatabase::GetItem(TEXT("Mana Fluid")))
        {
            FGameItem NewItem = *ManaFluid;
            NewItem.StackCount = 350;
            StartingItems.Add(NewItem);
        }

        if (const FGameItem *UltimateHealingRune = UItemDatabase::GetItem(TEXT("Ultimate Healing Rune")))
        {
            FGameItem NewItem = *UltimateHealingRune;
            NewItem.StackCount = 50;
            StartingItems.Add(NewItem);
        }

        
        if (const FGameItem *SuddenDeathRune = UItemDatabase::GetItem(TEXT("Sudden Death Rune")))
        {
            FGameItem NewItem = *SuddenDeathRune;
            NewItem.StackCount = 200;
            StartingItems.Add(NewItem);
        }

        if (const FGameItem *GreatFireballRune = UItemDatabase::GetItem(TEXT("Great Fireball Rune")))
        {
            FGameItem NewItem = *GreatFireballRune;
            NewItem.StackCount = 40;
            StartingItems.Add(NewItem);
        }
        if (const FGameItem *MagicWallRune = UItemDatabase::GetItem(TEXT("Magic Wall Rune")))
        {
            FGameItem NewItem = *MagicWallRune;
            NewItem.StackCount = 40;
            StartingItems.Add(NewItem);
        }

        if (const FGameItem *ParalyseRune = UItemDatabase::GetItem(TEXT("Paralyse Rune")))
        {
            FGameItem NewItem = *ParalyseRune;
            NewItem.StackCount = 40;
            StartingItems.Add(NewItem);
        }
        break;
    }

    case EVocation::Paladin:
    {
        if (const FGameItem *UltimateHealingRune = UItemDatabase::GetItem(TEXT("Ultimate Healing Rune")))
        {
            FGameItem NewItem = *UltimateHealingRune;
            NewItem.StackCount = 150;
            StartingItems.Add(NewItem);
        }
        if (const FGameItem *ManaFluid = UItemDatabase::GetItem(TEXT("Mana Fluid")))
        {
            FGameItem NewItem = *ManaFluid;
            NewItem.StackCount = 380;
            StartingItems.Add(NewItem);
        }
        if (const FGameItem *SuddenDeathRune = UItemDatabase::GetItem(TEXT("Sudden Death Rune")))
        {
            FGameItem NewItem = *SuddenDeathRune;
            NewItem.StackCount = 180;
            StartingItems.Add(NewItem);
        }
        if (const FGameItem *GreatFireballRune = UItemDatabase::GetItem(TEXT("Great Fireball Rune")))
        {
            FGameItem NewItem = *GreatFireballRune;
            NewItem.StackCount = 40;
            StartingItems.Add(NewItem);
        }
        if (const FGameItem *ExplosionRune = UItemDatabase::GetItem(TEXT("Explosion Rune")))
        {
            FGameItem NewItem = *ExplosionRune;
            NewItem.StackCount = 40;
            StartingItems.Add(NewItem);
        }
        if (const FGameItem *MagicWallRune = UItemDatabase::GetItem(TEXT("Magic Wall Rune")))
        {
            FGameItem NewItem = *MagicWallRune;
            NewItem.StackCount = 40;
            StartingItems.Add(NewItem);
        }
        if (const FGameItem *InfernalArrow = UItemDatabase::GetItem(TEXT("Infernal Arrow")))
        {
            FGameItem NewInfernalArrows = *InfernalArrow;
            NewInfernalArrows.StackCount = 100;
            StartingItems.Add(NewInfernalArrows);
        }
        break;
    }

    case EVocation::Knight:
    {
        if (const FGameItem *UltimateHealingRune = UItemDatabase::GetItem(TEXT("Ultimate Healing Rune")))
        {
            FGameItem NewItem = *UltimateHealingRune;
            NewItem.StackCount = 190;
            StartingItems.Add(NewItem);
        }
        if (const FGameItem *ManaFluid = UItemDatabase::GetItem(TEXT("Mana Fluid")))
        {
            FGameItem NewItem = *ManaFluid;
            NewItem.StackCount = 400;
            StartingItems.Add(NewItem);
        }
        if (const FGameItem *GreatFireballRune = UItemDatabase::GetItem(TEXT("Great Fireball Rune")))
        {
            FGameItem NewItem = *GreatFireballRune;
            NewItem.StackCount = 70;
            StartingItems.Add(NewItem);
        }
        if (const FGameItem *ExplosionRune = UItemDatabase::GetItem(TEXT("Explosion Rune")))
        {
            FGameItem NewItem = *ExplosionRune;
            NewItem.StackCount = 100;
            StartingItems.Add(NewItem);
        }
        if (const FGameItem *MagicWallRune = UItemDatabase::GetItem(TEXT("Magic Wall Rune")))
        {
            FGameItem NewItem = *MagicWallRune;
            NewItem.StackCount = 70;
            StartingItems.Add(NewItem);
        }
        break;
    }
    }

    return StartingItems;
}

int32 UPlayerStartingElements::GetStartingMagicLevel(EVocation Vocation)
{
    switch (Vocation)
    {
    case EVocation::Sorcerer:
    case EVocation::Druid:
        return 65;
    case EVocation::Paladin:
        return 21;
    case EVocation::Knight:
    default:
        return 8;
    }
}

int32 UPlayerStartingElements::GetStartingMeleeSkill(EVocation Vocation)
{
    switch (Vocation)
    {
    case EVocation::Knight:
        return 65;
    case EVocation::Paladin:
        return 10;
    case EVocation::Sorcerer:
    case EVocation::Druid:
    default:
        return 10;
    }
}

int32 UPlayerStartingElements::GetStartingDistanceSkill(EVocation Vocation)
{
    switch (Vocation)
    {
    case EVocation::Paladin:
        return 65;
    case EVocation::Knight:
        return 20;
    case EVocation::Sorcerer:
    case EVocation::Druid:
    default:
        return 10;
    }
}

int32 UPlayerStartingElements::GetStartingShieldingSkill(EVocation Vocation)
{
    switch (Vocation)
    {
    case EVocation::Knight:
        return 80;
    case EVocation::Paladin:
        return 40;
    case EVocation::Sorcerer:
    case EVocation::Druid:
    default:
        return 20;
    }
}

TArray<FGameItem> UPlayerStartingElements::GetTeamStashStartingItems()
{
    TArray<FGameItem> StashItems;

    if (const FGameItem *ManaFluid = UItemDatabase::GetItem(TEXT("Mana Fluid")))
    {
        FGameItem NewItem = *ManaFluid;
        NewItem.StackCount = 15000;
        StashItems.Add(NewItem);
    }

    if (const FGameItem *UltimateHealingRune = UItemDatabase::GetItem(TEXT("Ultimate Healing Rune")))
    {
        FGameItem NewItem = *UltimateHealingRune;
        NewItem.StackCount = 5000;
        StashItems.Add(NewItem);
    }

    if (const FGameItem *SuddenDeathRune = UItemDatabase::GetItem(TEXT("Sudden Death Rune")))
    {
        FGameItem NewItem = *SuddenDeathRune;
        NewItem.StackCount = 5000;
        StashItems.Add(NewItem);
    }

    if (const FGameItem *GreatFireballRune = UItemDatabase::GetItem(TEXT("Great Fireball Rune")))
    {
        FGameItem NewItem = *GreatFireballRune;
        NewItem.StackCount = 5000;
        StashItems.Add(NewItem);
    }

    if (const FGameItem *ExplosionRune = UItemDatabase::GetItem(TEXT("Explosion Rune")))
    {
        FGameItem NewItem = *ExplosionRune;
        NewItem.StackCount = 5000;
        StashItems.Add(NewItem);
    }

    if (const FGameItem *MagicWallRune = UItemDatabase::GetItem(TEXT("Magic Wall Rune")))
    {
        FGameItem NewItem = *MagicWallRune;
        NewItem.StackCount = 5000;
        StashItems.Add(NewItem);
    }

    if (const FGameItem *Arrow = UItemDatabase::GetItem(TEXT("Arrow")))
    {
        FGameItem NewItem = *Arrow;
        NewItem.StackCount = 25000;
        StashItems.Add(NewItem);
    }

    if (const FGameItem *InfernalArrow = UItemDatabase::GetItem(TEXT("Infernal Arrow")))
    {
        FGameItem NewInfernalArrows = *InfernalArrow;
        NewInfernalArrows.StackCount = 4000;
        StashItems.Add(NewInfernalArrows);
    }

    if (const FGameItem *ParalyseRune = UItemDatabase::GetItem(TEXT("Paralyse Rune")))
    {
        FGameItem NewItem = *ParalyseRune;
        NewItem.StackCount = 500;
        StashItems.Add(NewItem);
    }

    return StashItems;
}
